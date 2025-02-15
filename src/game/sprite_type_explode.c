/* sprite_type_explode.c
 * Decorative explosion.
 */
 
#include "presto.h"

#define CLOCK sprite->fv[0]

#define TTL 2.000

static void _explode_update(struct sprite *sprite,double elapsed) {
  if ((CLOCK+=elapsed)>=TTL) {
    sprite->defunct=1;
  }
}

static void _explode_render(struct sprite *sprite) {
  //TODO tint
  int dstx=(int)(sprite->x*NS_sys_tilesize)-NS_sys_tilesize;
  int dsty=(int)(sprite->y*NS_sys_tilesize)-NS_sys_tilesize;
  int srcx=0x0c*NS_sys_tilesize;
  int srcy=0x0e*NS_sys_tilesize;
  int w=NS_sys_tilesize<<1;
  int h=NS_sys_tilesize<<1;
  graf_draw_decal(&g.graf,g.texid_tiles,dstx,dsty,srcx,srcy,w,h,0);
  srcx+=NS_sys_tilesize<<1;
  graf_draw_decal(&g.graf,g.texid_tiles,dstx,dsty,srcx,srcy,w,h,0);
}

const struct sprite_type sprite_type_explode={
  .name="explode",
  .update=_explode_update,
  .render=_explode_render,
};
