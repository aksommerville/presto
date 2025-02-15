#include "presto.h"

/* Drop defunct sprites from the list.
 */
 
void sprites_drop_defunct() {
  int i=g.spritec;
  while (i-->0) {
    if (!g.spritev[i].defunct) continue;
    int rmc=1;
    while ((i>0)&&g.spritev[i-1].defunct) { i--; rmc++; }
    g.spritec-=rmc;
    memmove(g.spritev+i,g.spritev+i+rmc,sizeof(struct sprite)*(g.spritec-i));
  }
}

/* New sprite.
 */
 
static struct sprite *sprite_init(struct sprite *sprite,const struct sprite_type *type,double x,double y) {
  memset(sprite,0,sizeof(struct sprite));
  sprite->type=type;
  sprite->x=x;
  sprite->y=y;
  sprite->radius=0.5;
  sprite->terminal_velocity=TERMINAL_VELOCITY_DEFAULT;
  if (type->init&&(type->init(sprite)<0)) {
    sprite->defunct=1;
    return 0;
  }
  return sprite;
}
 
struct sprite *sprite_new(const struct sprite_type *type,double x,double y) {
  if (!type) return 0;
  if (g.spritec>=SPRITE_LIMIT) return 0;
  struct sprite *sprite=g.spritev+g.spritec++;
  return sprite_init(sprite,type,x,y);
}
 
int sprite_retype(struct sprite *sprite,const struct sprite_type *type) {
  if (!sprite||!type) return -1;
  if (sprite->type==type) return 0;
  uint8_t xform=sprite->xform;
  if (!sprite_init(sprite,type,sprite->x,sprite->y)) return -1;
  sprite->xform=xform;
  if (sprite->type->after_retype) sprite->type->after_retype(sprite);
  return 0;
}

/* Search sprite list.
 */

struct sprite *sprites_any_of_type(const struct sprite_type *type) {
  if (!type) return 0;
  struct sprite *sprite=g.spritev;
  int i=g.spritec;
  for (;i-->0;sprite++) {
    if (sprite->type==type) return sprite;
  }
  return 0;
}

/* Transformable.
 */
 
int sprite_is_transformable(const struct sprite *sprite) {
  if (!sprite) return 0;
  if (
    (sprite->type!=&sprite_type_pumpkin)&&
    (sprite->type!=&sprite_type_balloon)&&
    (sprite->type!=&sprite_type_bomb)&&
    (sprite->type!=&sprite_type_hippopotamus)
  ) return 0;
  int x=(int)sprite->x;
  int y=(int)sprite->y;
  if ((x>=0)&&(y>=0)&&(x<NS_sys_mapw)&&(y<NS_sys_maph)) {
    uint8_t tileid=g.map[y*NS_sys_mapw+x];
    uint8_t physics=g.physics[tileid];
    if (physics==NS_physics_timemachine) return 0;
  }
  return 1;
}

uint8_t tileid_for_carry(const struct sprite *sprite) {
  if (sprite->type==&sprite_type_pumpkin) return 0xe0;
  if (sprite->type==&sprite_type_balloon) return 0xe1;
  if (sprite->type==&sprite_type_bomb) return 0xe2;
  if (sprite->type==&sprite_type_hippopotamus) return 0xe3;
  return 0;
}

/* Get sprite type.
 */
 
const struct sprite_type *sprite_type_by_id(int id) {
  switch (id) {
    #define _(tag) case NS_spritetype_##tag: return &sprite_type_##tag;
    FOR_EACH_spritetype
    #undef _
  }
  return 0;
}
