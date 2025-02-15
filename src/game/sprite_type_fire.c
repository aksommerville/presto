#include "presto.h"

#define ANIMCLOCK sprite->fv[0]
#define ANIMFRAME sprite->iv[0]

static void _fire_update(struct sprite *sprite,double elapsed) {
  if ((ANIMCLOCK-=elapsed)<=0.0) {
    ANIMCLOCK+=0.125;
    if (++(ANIMFRAME)>=4) ANIMFRAME=0;
  }
  switch (ANIMFRAME) {
    case 0: sprite->tileid=0xf0; sprite->xform=0; break;
    case 1: sprite->tileid=0xf1; sprite->xform=0; break;
    case 2: sprite->tileid=0xf0; sprite->xform=EGG_XFORM_XREV; break;
    case 3: sprite->tileid=0xf1; sprite->xform=EGG_XFORM_XREV; break;
  }
  
  double radius=0.75;
  double l=sprite->x-radius;
  double r=sprite->x+radius;
  double t=sprite->y-radius;
  double b=sprite->y+radius;
  struct sprite *victim=g.spritev;
  int i=g.spritec;
  for (;i-->0;victim++) {
    if (victim->defunct) continue;
    if (victim->x<l) continue;
    if (victim->x>r) continue;
    if (victim->y<t) continue;
    if (victim->y>b) continue;
    
    if (victim->type==&sprite_type_hero) {
      egg_play_sound(RID_sound_hurt);
      victim->defunct=1;
      generate_soulballs(victim->x,victim->y,7);
      fail_level();
      
    } else if (victim->type==&sprite_type_bomb) {
      egg_play_sound(RID_sound_hurt);
      victim->defunct=1;
      explode(victim->x,victim->y);
      
    } else if (
      (victim->type==&sprite_type_balloon)||
      (victim->type==&sprite_type_hippopotamus)
    ) {
      egg_play_sound(RID_sound_hurt);
      victim->defunct=1;
      generate_soulballs(victim->x,victim->y,6);
    }
  }
}

const struct sprite_type sprite_type_fire={
  .name="fire",
  .update=_fire_update,
};
