/* sprite_type_explode.c
 * Decorative explosion.
 */
 
#include "presto.h"

#define CLOCK sprite->fv[0]

#define TTL 2.000 /* s */
#define BG_FADE_TIME 0.500 /* s */
#define BG_COLOR_RATE 8.0 /* hz */

static const uint32_t colorv[8]={
  0xffff00ff,
  0xff0000ff,
  0xff8000ff,
  0x00ff00ff,
  0xc0ff40ff,
  0xc08000ff,
  0x00ffffff,
  0xff00ffff,
};

static void _explode_update(struct sprite *sprite,double elapsed) {
  if ((CLOCK+=elapsed)>=TTL) {
    sprite->defunct=1;
  }
}

static void _explode_render(struct sprite *sprite) {
  int dstx=(int)(sprite->x*NS_sys_tilesize)-NS_sys_tilesize;
  int dsty=(int)(sprite->y*NS_sys_tilesize)-NS_sys_tilesize;
  int srcx=0x0c*NS_sys_tilesize;
  int srcy=0x0e*NS_sys_tilesize;
  int w=NS_sys_tilesize<<1;
  int h=NS_sys_tilesize<<1;
  
  // Background toggles XREV and tint, and fades out near the end.
  double remaining=TTL-CLOCK;
  if (remaining<BG_FADE_TIME) {
    int alpha=(int)((remaining*192.0)/BG_FADE_TIME);
    if (alpha<0) alpha=0; else if (alpha>0xff) alpha=0xff;
    graf_set_alpha(&g.graf,alpha);
  } else {
    graf_set_alpha(&g.graf,0xc0);
  }
  int color=((int)(CLOCK*BG_COLOR_RATE))&7;
  graf_set_tint(&g.graf,colorv[color]);
  uint8_t xform=0;
  if (color&2) xform=EGG_XFORM_XREV;
  graf_draw_decal(&g.graf,g.texid_tiles,dstx,dsty,srcx,srcy,w,h,xform);
  graf_set_alpha(&g.graf,0xff);
  graf_set_tint(&g.graf,0);
  
  // "POW" drifts upward.
  srcx+=NS_sys_tilesize<<1;
  dsty+=4;
  dsty-=(int)(CLOCK*8.0);
  graf_draw_decal(&g.graf,g.texid_tiles,dstx,dsty,srcx,srcy,w,h,0);
}

const struct sprite_type sprite_type_explode={
  .name="explode",
  .update=_explode_update,
  .render=_explode_render,
};
