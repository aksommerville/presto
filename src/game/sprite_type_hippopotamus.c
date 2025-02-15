#include "presto.h"

static int _hippopotamus_init(struct sprite *sprite) {
  sprite->tileid=0xe3;
  return 0;
}

static void _hippopotamus_update(struct sprite *sprite,double elapsed) {
  sprite_gravity(sprite,elapsed);
}

const struct sprite_type sprite_type_hippopotamus={
  .name="hippopotamus",
  .init=_hippopotamus_init,
  .update=_hippopotamus_update,
};
