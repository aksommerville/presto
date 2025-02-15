/* sprite.h
 */
 
#ifndef SPRITE_H
#define SPRITE_H

#define SPRITE_LIMIT 128

#define SPRITE_MODE_NORMAL 1 /* Single tile. */
#define SPRITE_MODE_TALL   2 /* Draw a second tile above me. */

struct sprite {
  int defunct;
  double x,y;
  uint8_t tileid;
  uint8_t xform;
  int mode;
};

void sprites_drop_defunct();

struct sprite *sprite_new();

#endif
