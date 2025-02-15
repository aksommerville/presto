#include "presto.h"

#define JUMP_POWER sprite->fv[0]
#define CARRY sprite->iv[0]

#define JUMP_INITIAL      25.000
#define JUMP_REDUCE_RATE 100.000
#define WALK_SPEED 8.0
#define TERMINAL_VELOCITY_BALLOON 3.0

static int _hero_init(struct sprite *sprite) {
  sprite->tileid=0xd0;
  return 0;
}

static void _hero_update(struct sprite *sprite,double elapsed) {

  /* Left and right to walk.
   * Nothing fancy here. Constant speed, etc.
   */
  switch (g.pvinput&(EGG_BTN_LEFT|EGG_BTN_RIGHT)) {
    case EGG_BTN_LEFT: {
        sprite->xform=EGG_XFORM_XREV;
        sprite_move(sprite,-WALK_SPEED*elapsed,0.0);
      } break;
    case EGG_BTN_RIGHT: {
        sprite->xform=0;
        sprite_move(sprite,WALK_SPEED*elapsed,0.0);
      } break;
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
    }
  }
}

static void _hero_render(struct sprite *sprite) {
  int16_t dstx=(int16_t)(sprite->x*NS_sys_tilesize);
  int16_t dsty=(int16_t)(sprite->y*NS_sys_tilesize);
  graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty,sprite->tileid,sprite->xform);
  graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty-NS_sys_tilesize,sprite->tileid-0x10,sprite->xform);
  if (CARRY) {
    int16_t cdstx=dstx;
    const int16_t cdx=12,cdy=4;
    if (sprite->xform) cdstx-=cdx;
    else cdstx+=cdx;
    int16_t cdsty=dsty-cdy;
    graf_draw_tile(&g.graf,g.texid_tiles,cdstx,cdsty,CARRY,sprite->xform);
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
    sprite_move(pumpkin,sprite->xform?-1.0:1.0,0.0);
    sprite_move(pumpkin,0.0,0.25);
    if (pumpkin->type->after_drop) pumpkin->type->after_drop(pumpkin);
    egg_play_sound(RID_sound_drop);
    CARRY=0;
    sprite->terminal_velocity=TERMINAL_VELOCITY_DEFAULT;
    return;
  }
  
  double ylo=sprite->y-1.5;
  double yhi=sprite->y+0.5;
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
  if (bestdx>1.5) return;
  
  egg_play_sound(RID_sound_pickup);
  CARRY=tileid_for_carry(best);
  best->defunct=1;
  if (CARRY==0xe1) { // balloon
    sprite->terminal_velocity=TERMINAL_VELOCITY_BALLOON;
  }
}

