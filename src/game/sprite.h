/* sprite.h
 */
 
#ifndef SPRITE_H
#define SPRITE_H

#define SPRITE_LIMIT 128
#define SPRITE_IV_SIZE 4
#define SPRITE_FV_SIZE 4

#define TERMINAL_VELOCITY_DEFAULT 14.0

struct sprite;
struct sprite_type;

struct sprite_type {
  const char *name;
  int (*init)(struct sprite *sprite);
  void (*update)(struct sprite *sprite,double elapsed);
  void (*render)(struct sprite *sprite);
  void (*after_drop)(struct sprite *sprite);
  void (*after_retype)(struct sprite *sprite);
};

struct sprite {
  const struct sprite_type *type;
  int defunct;
  double x,y;
  uint8_t tileid;
  uint8_t xform;
  int mode;
  double radius;
  double gravity; // For physics.
  double terminal_velocity;
  int iv[SPRITE_IV_SIZE];
  double fv[SPRITE_FV_SIZE];
};

void sprites_drop_defunct();

struct sprite *sprite_new(const struct sprite_type *type,double x,double y);

struct sprite *sprites_any_of_type(const struct sprite_type *type);

int sprite_retype(struct sprite *sprite,const struct sprite_type *type);

/* Checks type and position against map -- not transformable if we're standing in the time machine.
 */
int sprite_is_transformable(const struct sprite *sprite);

uint8_t tileid_for_carry(const struct sprite *sprite);

/* Apply motion.
 * If you want collisions, you must apply motion via these.
 * sprite_move() requires that one axis be zero; our collision detection is pretty naive.
 * Both return nonzero if there was a collision. Partial motion may still have been applied.
 */
int sprite_move(struct sprite *sprite,double dx,double dy);
int sprite_gravity(struct sprite *sprite,double elapsed);

const struct sprite_type *sprite_type_by_id(int id);
#define _(tag) extern const struct sprite_type sprite_type_##tag;
FOR_EACH_spritetype
#undef _

void sprite_hero_interact(struct sprite *sprite);
void sprite_hero_pickup(struct sprite *sprite);

#endif
