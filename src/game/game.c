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
  
  /* Process map commands.
   */
  struct rom_command_reader cmdreader={.v=rmap.cmdv,.c=rmap.cmdc};
  struct rom_command cmd;
  while (rom_command_reader_next(&cmd,&cmdreader)>0) {
    switch (cmd.opcode) {
      case CMD_map_sprite: {
          uint8_t x=cmd.argv[0];
          uint8_t y=cmd.argv[1];
          uint16_t spritetype=(cmd.argv[2]<<8)|cmd.argv[3];
          struct sprite *sprite=sprite_new(sprite_type_by_id(spritetype),x+0.5,y+0.5);
        } break;
    }
  }
  
  /* Force a universe change.
   * We always start in the real universe, ie the Pumpkinverse.
   */
  g.universe=-1;
  set_universe(NS_uv_pumpkin);
}

/* Choose a different universe.
 * This really ought to be a modal, but for now just toggle thru them.
 */
 
void begin_universe_selection() {
  set_universe((g.universe+1)&3);
}

/* Change universe.
 */
 
void set_universe(int uv) {
  if ((uv==g.universe)||(uv<0)||(uv>3)) return;
  g.universe=uv;
  g.bg_dirty=1;
  const struct sprite_type *type=0;
  switch (uv) {
    case NS_uv_pumpkin: {
        type=&sprite_type_pumpkin;
        egg_play_song(RID_song_just_add_fire,0,1);
      } break;
    case NS_uv_balloon: {
        type=&sprite_type_balloon;
        egg_play_song(RID_song_look_at_him_go,0,1);
      } break;
    case NS_uv_bomb: {
        type=&sprite_type_bomb;
        egg_play_song(RID_song_bouncing_star,0,1);
      } break;
    case NS_uv_hippopotamus: {
        type=&sprite_type_hippopotamus;
        egg_play_song(RID_song_shiny_river,0,1);
      } break;
  }
  if (type) {
    struct sprite *sprite=g.spritev;
    int i=g.spritec;
    for (;i-->0;sprite++) {
      if (sprite_is_transformable(sprite)) {
        sprite_retype(sprite,type);
      }
    }
  }
}
