#include "presto.h"

#define TTL sprite->fv[0]

#define TTL_INIT 5.000

static int _bomb_init(struct sprite *sprite) {
  sprite->tileid=0xe6;
  TTL=TTL_INIT;
  return 0;
}

static void _bomb_update(struct sprite *sprite,double elapsed) {
  sprite_gravity(sprite,elapsed);
  if ((TTL-=elapsed)<=0.0) {
    sprite->defunct=1;
    explode(sprite->x,sprite->y);
  } else {
    const double interval=TTL_INIT/5.0;
    int frame=(int)(TTL/interval);
    if (frame<0) frame=0; else if (frame>=5) frame=4;
    sprite->tileid=0xea-frame;
  }
}

static void _bomb_after_drop(struct sprite *sprite) {
  // Hero will try to xform us after dropping, to match the hero's direction.
  // But our tiles contain text and must not transform.
  //...well actually no, we absolutely must preserve xform. So instead, we'll override render.
  //sprite->xform=0;
}

static void _bomb_render(struct sprite *sprite) {
  int16_t dstx=(int)(sprite->x*NS_sys_tilesize);
  int16_t dsty=(int)(sprite->y*NS_sys_tilesize);
  graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty,sprite->tileid,0);
}

const struct sprite_type sprite_type_bomb={
  .name="bomb",
  .init=_bomb_init,
  .update=_bomb_update,
  .render=_bomb_render,
};
