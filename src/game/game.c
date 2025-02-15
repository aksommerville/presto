#include "presto.h"

/* Begin level.
 */
 
void begin_level(int id) {
  fprintf(stderr,"%s %d\n",__func__,id);
  g.spritec=0;
  g.universe=NS_uv_pumpkin;
  
  /* Load cells from the map resource.
   */
  const void *serial=0;
  int serialc=0;
  struct rom_reader reader;
  if (rom_reader_init(&reader,g.rom,g.romc)<0) return;
  struct rom_res *res;
  while (res=rom_reader_next(&reader)) {
    switch (res->tid) {
      case EGG_TID_map: if (res->rid==id) { serial=res->v; serialc=res->c; break; }
    }
    if (serial) break;
  }
  if (!serial) {
    fprintf(stderr,"map:%d not found\n",id);
    egg_terminate(1);
    return;
  }
  struct rom_map rmap={0};
  if (rom_map_decode(&rmap,serial,serialc)<0) return;
  if ((rmap.w!=NS_sys_mapw)||(rmap.h!=NS_sys_maph)) return;
  memcpy(g.map,rmap.v,NS_sys_mapw*NS_sys_maph);
  g.bg_dirty=1;
  
  //TODO Read map commands. Spawn sprites.
  
  struct sprite *hero=sprite_new();
  if (!hero) return;
  hero->x=4.5;
  hero->y=9.5;
  hero->tileid=0xd0;
  hero->mode=SPRITE_MODE_TALL;
  
  egg_play_song(RID_song_shiny_river,0,1);
}
