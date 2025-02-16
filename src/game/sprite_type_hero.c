#include "presto.h"

#define JUMP_POWER sprite->fv[0]
#define ANIMCLOCK sprite->fv[1]
#define CARRY sprite->iv[0]
#define ANIMFRAME sprite->iv[1]

#define JUMP_INITIAL      25.000
#define JUMP_REDUCE_RATE 100.000
#define WALK_SPEED 8.0
#define TERMINAL_VELOCITY_BALLOON 3.0

static int _hero_init(struct sprite *sprite) {
  sprite->tileid=0xd0;
  sprite->footed=1;
  return 0;
}

static void _hero_update(struct sprite *sprite,double elapsed) {

  if (g.celebration>0.0) return;

  /* Left and right to walk.
   * Nothing fancy here. Constant speed, etc.
   */
  int walking=0;
  switch (g.pvinput&(EGG_BTN_LEFT|EGG_BTN_RIGHT)) {
    case EGG_BTN_LEFT: {
        sprite->xform=EGG_XFORM_XREV;
        sprite_move(sprite,-WALK_SPEED*elapsed,0.0);
        walking=1;
      } break;
    case EGG_BTN_RIGHT: {
        sprite->xform=0;
        sprite_move(sprite,WALK_SPEED*elapsed,0.0);
        walking=1;
      } break;
  }
  if (walking) {
    if ((ANIMCLOCK-=elapsed)<=0.0) {
      ANIMCLOCK+=0.200;
      if (++(ANIMFRAME)>=2) ANIMFRAME=0;
    }
  } else {
    ANIMCLOCK=0.0;
    ANIMFRAME=0;
  }
  
  /* Jump if we're doing that, and gravity if not.
   */
  int enable_gravity=1;
  if ((g.pvinput&EGG_BTN_SOUTH)&&(JUMP_POWER>0.0)) {
    if (JUMP_POWER==JUMP_INITIAL) {
      egg_play_sound(RID_sound_jump);
    }
    sprite_move(sprite,0.0,-JUMP_POWER*elapsed);
    JUMP_POWER-=JUMP_REDUCE_RATE*elapsed;
    enable_gravity=0;
  } else {
    if (sprite_gravity(sprite,elapsed)) {
      if (!(g.pvinput&EGG_BTN_SOUTH)) {
        JUMP_POWER=JUMP_INITIAL;
      }
    } else if (JUMP_POWER<JUMP_INITIAL) { // Stopped mid-jump. No restartsies.
      JUMP_POWER=0.0;
    } else if (sprite->gravity>2.0) { // Wait for gravity to kick in; allow some coyote time.
      JUMP_POWER=0.0;
    }
  }
  
  /* Create or destroy an advice sprite according to what we're standing near.
   */
  int advice=0;
  int col=(int)sprite->x;
  int row=(int)sprite->y;
  if ((col>=0)&&(row>=0)&&(col<NS_sys_mapw)&&(row<NS_sys_maph)) {
    uint8_t tile=g.map[row*NS_sys_mapw+col];
    switch (tile) {
      case 0xa1: advice=0xeb; break; // Altar Of Paralleleaping.
    }
  }
  if (advice) {
    struct sprite *advisor=sprite_advice_require();
    if (advisor) {
      advisor->x=col+0.5;
      advisor->y=row-1.5;
      advisor->tileid=advice;
    }
  } else {
    sprite_advice_forbid();
  }
  
  /* Check completion.
   * There's no clock, instead we take a simplistic approach: Check only when standing still, and effect immediately.
   */
  if (!(g.pvinput&(EGG_BTN_LEFT|EGG_BTN_RIGHT))&&(JUMP_POWER>=JUMP_INITIAL)) {
    if ((col>=0)&&(row>=0)&&(col<NS_sys_mapw)&&(row<NS_sys_maph-1)) {
      uint8_t tileid=g.map[(row+1)*NS_sys_mapw+col];
      if (tileid<0x80) tileid=(tileid&0x37)|((g.universe&2)<<5)|((g.universe&1)<<3);
      uint8_t physics=g.physics[tileid];
      if (physics==NS_physics_goal) {
        if (CARRY) sprite_hero_pickup(sprite);
        win_level();
      }
    }
  }
}

static void _hero_render(struct sprite *sprite) {
  int16_t dstx=(int16_t)(sprite->x*NS_sys_tilesize);
  int16_t dsty=(int16_t)(sprite->y*NS_sys_tilesize);
  uint8_t tileid=sprite->tileid;
  if (g.celebration>0.0) tileid=0xd2;
  else if (ANIMFRAME) tileid=0xd1;
  graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty,tileid,sprite->xform);
  graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty-NS_sys_tilesize,tileid-0x10,sprite->xform);
  if (g.celebration>0.0) return;
  if (CARRY) {
    int16_t cdstx=dstx;
    const int16_t cdx=11,cdy=2;
    if (sprite->xform) cdstx-=cdx;
    else cdstx+=cdx;
    int16_t cdsty=dsty-cdy;
    graf_draw_tile(&g.graf,g.texid_tiles,cdstx,cdsty,CARRY,sprite->xform);
    graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty,0xd5,sprite->xform);
  } else {
    graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty,0xd3+ANIMFRAME,sprite->xform);
  }
}

const struct sprite_type sprite_type_hero={
  .name="hero",
  .init=_hero_init,
  .update=_hero_update,
  .render=_hero_render,
};

void sprite_hero_interact(struct sprite *sprite) {
  if (!sprite||(sprite->type!=&sprite_type_hero)) return;
  int x=(int)sprite->x;
  int y=(int)sprite->y;
  if ((x<0)||(x>=NS_sys_mapw)||(y<0)||(y>=NS_sys_maph)) return;
  uint8_t tileid=g.map[y*NS_sys_mapw+x];
  if (tileid==0xa1) { // Ought to use a map command for this, but whatever, there's just one tilesheet.
    begin_universe_selection();
    return;
  }
}

void sprite_hero_pickup(struct sprite *sprite) {
  if (!sprite||(sprite->type!=&sprite_type_hero)) return;
  
  if (CARRY) {
    const struct sprite_type *type=0;
    switch (CARRY) {
      case 0xe0: type=&sprite_type_pumpkin; break;
      case 0xe1: type=&sprite_type_balloon; break;
      case 0xe2: type=&sprite_type_bomb; break;
      case 0xe3: type=&sprite_type_hippopotamus; break;
    }
    struct sprite *pumpkin=sprite_new(type,sprite->x,sprite->y-0.5);
    if (!pumpkin) return;
    pumpkin->xform=sprite->xform;
    // Move instead of placing in the target spot initially, to let physics interfere.
    sprite_move(pumpkin,0.0,0.5);
    sprite_move(pumpkin,sprite->xform?-1.0:1.0,0.0);
    if (pumpkin->type->after_drop) pumpkin->type->after_drop(pumpkin);
    egg_play_sound(RID_sound_drop);
    CARRY=0;
    sprite->terminal_velocity=TERMINAL_VELOCITY_DEFAULT;
    return;
  }
  
  double ylo=sprite->y-1.75;
  double yhi=sprite->y+0.75;
  double xtarget=sprite->x;
  if (sprite->xform) xtarget-=0.5; else xtarget+=0.5;
  struct sprite *best=0;
  double bestdx;
  struct sprite *pumpkin=g.spritev;
  int i=g.spritec;
  for (;i-->0;pumpkin++) {
    if (pumpkin->y<ylo) continue;
    if (pumpkin->y>yhi) continue;
    if (
      (pumpkin->type!=&sprite_type_pumpkin)&&
      (pumpkin->type!=&sprite_type_balloon)&&
      (pumpkin->type!=&sprite_type_bomb)&&
      (pumpkin->type!=&sprite_type_hippopotamus)
    ) continue;
    double dx=pumpkin->x-xtarget;
    if (dx<0.0) dx=-dx;
    if (best) {
      if (dx>bestdx) continue;
    }
    best=pumpkin;
    bestdx=dx;
  }
  if (!best) return;
  if (bestdx>2.0) return;
  
  egg_play_sound(RID_sound_pickup);
  CARRY=tileid_for_carry(best);
  best->defunct=1;
  if (CARRY==0xe1) { // balloon
    sprite->terminal_velocity=TERMINAL_VELOCITY_BALLOON;
  }
}

