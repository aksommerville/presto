#include "presto.h"

static int _bomb_init(struct sprite *sprite) {
  sprite->tileid=0xe2;
  return 0;
}

static void _bomb_update(struct sprite *sprite,double elapsed) {
  sprite_gravity(sprite,elapsed);
}

const struct sprite_type sprite_type_bomb={
  .name="bomb",
  .init=_bomb_init,
  .update=_bomb_update,
};
