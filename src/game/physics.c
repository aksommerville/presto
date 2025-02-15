#include "presto.h"

#define GRAVITY_ACCELERATION 30.0

/* Nonzero if any cell in this range is impassable.
 * Caller validates arguments, we don't.
 */
 
static int sprite_map_collision(const struct sprite *sprite,int cola,int colz,int rowa,int rowz) {
  const uint8_t *cellrow=g.map+rowa*NS_sys_mapw+cola;
  int row=rowa; for (;row<=rowz;row++,cellrow+=NS_sys_mapw) {
    const uint8_t *cellp=cellrow;
    int col=cola; for (;col<=colz;col++,cellp++) {
      uint8_t physics=g.physics[*cellp];
      if ((physics==NS_physics_solid)||(physics==NS_physics_goal)) return 1;
    }
  }
  return 0;
}

/* Move.
 */
 
int sprite_move(struct sprite *sprite,double dx,double dy) {
  int collided=0;
  sprite->x+=dx;
  sprite->y+=dy;
  
  /* No sprite is allowed to leave the screen.
   */
  if (sprite->x<sprite->radius) { sprite->x=sprite->radius; collided=1; }
  else if (sprite->x>NS_sys_mapw-sprite->radius) { sprite->x=NS_sys_mapw-sprite->radius; collided=1; }
  if (sprite->y<sprite->radius) { sprite->y=sprite->radius; collided=1; }
  else if (sprite->y>NS_sys_maph-sprite->radius) { sprite->y=NS_sys_maph-sprite->radius; collided=1; }
  
  /* Determine map coverage on the still axis, and starting point on the moving axis.
   */
  int cola,colz,rowa,rowz,idx=0,idy=0;
  if ((dx<0.0)||(dx>0.0)) {
    cola=colz=(int)sprite->x;
    idx=(dx<0.0)?-1:1;
    rowa=(int)(sprite->y-sprite->radius+0.001); if (rowa<0) rowa=0;
    rowz=(int)(sprite->y+sprite->radius-0.001); if (rowz>=NS_sys_maph) rowz=NS_sys_maph-1;
  } else {
    rowa=rowz=(int)sprite->y;
    idy=(dy<0.0)?-1:1;
    cola=(int)(sprite->x-sprite->radius+0.001); if (cola<0) cola=0;
    colz=(int)(sprite->x+sprite->radius-0.001); if (colz>=NS_sys_mapw) colz=NS_sys_mapw-1;
  }
  
  /* Walk that window until it goes offscreen.
   * First time it finds something impassable, stop there and correct to the leading edge.
   */
  int collided_map=0;
  int ckc=2; // How many steps. 2 should always be enough, since our sprites are no wider than a tile.
  while ((cola>=0)&&(rowa>=0)&&(colz<NS_sys_mapw)&&(rowz<NS_sys_maph)&&(ckc-->0)) {
    if (sprite_map_collision(sprite,cola,colz,rowa,rowz)) {
      collided_map=1;
      break;
    }
    cola+=idx;
    colz+=idx;
    rowa+=idy;
    rowz+=idy;
  }
  if (collided_map) {
    if (dx<0.0) {
      double n=cola+1.0+sprite->radius;
      if (n>sprite->x) { sprite->x=n; collided=1; }
    } else if (dx>0.0) {
      double n=cola-sprite->radius;
      if (n<sprite->x) { sprite->x=n; collided=1; }
    } else if (dy<0.0) {
      double n=rowa+1.0+sprite->radius;
      if (n>sprite->y) { sprite->y=n; collided=1; }
    } else {
      double n=rowa-sprite->radius;
      if (n<sprite->y) { sprite->y=n; collided=1; }
    }
  }
  
  return collided;
}

/* Apply gravity.
 */
 
int sprite_gravity(struct sprite *sprite,double elapsed) {
  sprite->gravity+=GRAVITY_ACCELERATION*elapsed;
  if (sprite->gravity>sprite->terminal_velocity) {
    sprite->gravity=sprite->terminal_velocity;
  }
  if (sprite_move(sprite,0.0,sprite->gravity*elapsed)) {
    sprite->gravity=0.0;
    return 1;
  }
  return 0;
}
