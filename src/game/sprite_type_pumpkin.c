#include "presto.h"

static int _pumpkin_init(struct sprite *sprite) {
  sprite->tileid=0xe0;
  return 0;
}

static void _pumpkin_update(struct sprite *sprite,double elapsed) {
  sprite_gravity(sprite,elapsed);
}

const struct sprite_type sprite_type_pumpkin={
  .name="pumpkin",
  .init=_pumpkin_init,
  .update=_pumpkin_update,
};
