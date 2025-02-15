/* sprite_type_soulballs.c
 * One sprite renders all the balls.
 * Caller should set iv[0] at init to the desired count.
 * (a witch's soul has 7 circles; nothing else in nature has a soul of more than 6 circles).
 */
 
#include "presto.h"

#define CIRCLEC sprite->iv[0]
#define TTL sprite->fv[0]
#define RADIUS sprite->fv[1]
#define ANIMCLOCK sprite->fv[2]

#define FADE_TIME 1.000
#define EXPAND_RATE 4.0

static int _soulballs_init(struct sprite *sprite) {
  CIRCLEC=5;
  TTL=2.0;
  sprite->tileid=0xf2;
  return 0;
}

static void _soulballs_update(struct sprite *sprite,double elapsed) {
  if ((TTL-=elapsed)<=0.0) {
    sprite->defunct=1;
    return;
  }
  if ((ANIMCLOCK-=elapsed)<=0.0) {
    ANIMCLOCK+=0.100;
    if (++(sprite->tileid)>=0xf8) sprite->tileid=0xf2;
  }
  RADIUS+=EXPAND_RATE*elapsed;
}

static void _soulballs_render(struct sprite *sprite) {
  if ((CIRCLEC<1)||(CIRCLEC>7)) return;
  
  if (TTL<FADE_TIME) {
    int alpha=(int)((TTL*256.0)/FADE_TIME);
    if (alpha<1) return;
    if (alpha<0xff) graf_set_alpha(&g.graf,alpha);
  }
  
  uint8_t tileid=sprite->tileid;
  double t=0.0,dt=(M_PI*2.0)/CIRCLEC;
  int i=CIRCLEC;
  for (;i-->0;t+=dt,tileid++) {
    if (tileid>=0xf8) tileid=0xf2;
    double x=sprite->x+RADIUS*cos(t);
    double y=sprite->y-RADIUS*sin(t);
    int dstx=(int)(x*NS_sys_tilesize);
    int dsty=(int)(y*NS_sys_tilesize);
    graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty,tileid,0);
  }
  
  graf_set_alpha(&g.graf,0xff);
}

const struct sprite_type sprite_type_soulballs={
  .name="soulballs",
  .init=_soulballs_init,
  .update=_soulballs_update,
  .render=_soulballs_render,
};
