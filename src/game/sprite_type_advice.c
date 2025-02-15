/* sprite_type_advice.c
 * A little sprite that indicates "press up" at appropriate moments.
 * Mostly managed by the hero.
 */
 
#include "presto.h"

const struct sprite_type sprite_type_advice={
  .name="advice",
};

struct sprite *sprite_advice_require() {
  struct sprite *q=g.spritev;
  int i=g.spritec;
  for (;i-->0;q++) {
    if (q->type==&sprite_type_advice) {
      q->defunct=0;
      return q;
    }
  }
  return sprite_new(&sprite_type_advice,0.0,0.0);
}

void sprite_advice_forbid() {
  struct sprite *q=g.spritev;
  int i=g.spritec;
  for (;i-->0;q++) {
    if (q->type==&sprite_type_advice) {
      q->defunct=1;
    }
  }
}
