#include "presto.h"

#define FLOAT_SPEED -1.000

static int _balloon_init(struct sprite *sprite) {
  sprite->tileid=0xe1;
  return 0;
}

static void _balloon_update(struct sprite *sprite,double elapsed) {
  sprite_move(sprite,0,FLOAT_SPEED*elapsed);
}

const struct sprite_type sprite_type_balloon={
  .name="balloon",
  .init=_balloon_init,
  .update=_balloon_update,
};
