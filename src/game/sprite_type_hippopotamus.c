#include "presto.h"

#define ANIMCLOCK sprite->fv[0]
#define BLOCKTIME sprite->fv[1]
#define ANIMFRAME sprite->iv[0]

#define WALK_SPEED 3.0
#define TURNAROUND_TIME 0.500

static int _hippopotamus_init(struct sprite *sprite) {
  sprite->tileid=0xe3;
  sprite->footed=1;
  return 0;
}

static void _hippopotamus_update(struct sprite *sprite,double elapsed) {
  sprite_gravity(sprite,elapsed);
  if ((ANIMCLOCK-=elapsed)<=0.0) {
    ANIMCLOCK+=0.125;
    if (++(ANIMFRAME)>=4) ANIMFRAME=0;
    switch (ANIMFRAME) {
      case 0: case 2: sprite->tileid=0xe3; break;
      case 1: sprite->tileid=0xe4; break;
      case 3: sprite->tileid=0xe5; break;
    }
  }
  double dx=sprite->xform?-1.0:1.0;
  dx*=elapsed*WALK_SPEED;
  if (sprite_move(sprite,dx,0.0)) {
    BLOCKTIME+=elapsed;
    if (BLOCKTIME>=TURNAROUND_TIME) {
      sprite->xform^=EGG_XFORM_XREV;
    }
  } else {
    BLOCKTIME=0.0;
  }
}

const struct sprite_type sprite_type_hippopotamus={
  .name="hippopotamus",
  .init=_hippopotamus_init,
  .update=_hippopotamus_update,
};
