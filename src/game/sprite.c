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
 
struct sprite *sprite_new() {
  if (g.spritec>=SPRITE_LIMIT) return 0;
  struct sprite *sprite=g.spritev+g.spritec++;
  memset(sprite,0,sizeof(struct sprite));
  return sprite;
}
