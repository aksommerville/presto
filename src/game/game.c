#include "presto.h"

/* Begin level.
 */
 
void begin_level(int id) {
  g.spritec=0;
  g.universe=NS_uv_pumpkin;
  g.mapid=id;
  
  /* Load cells from the map resource.
   */
  const void *serial=0,*s1=0;
  int serialc=0,s1c=0;
  struct rom_reader reader;
  if (rom_reader_init(&reader,g.rom,g.romc)<0) return;
  struct rom_res *res;
  while (res=rom_reader_next(&reader)) {
    switch (res->tid) {
      case EGG_TID_map: {
          if (res->rid==id) { serial=res->v; serialc=res->c; }
          else if (res->rid==1) { s1=res->v; s1c=res->c; }
        } break;
    }
    if (serial) break;
  }
  if (!serial) {
    if (!s1) { egg_terminate(1); return; }
    fprintf(stderr,"End of maps. Restarting from map:1.\n");
    g.mapid=1;
    serial=s1;
    serialc=s1c;
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

void reset_level() {
  begin_level(g.mapid);
}

/* Finish level, load the next one.
 */
 
void win_level() {
  egg_play_sound(RID_sound_win);
  egg_play_song(0,0,1);
  g.celebration=2.000;
}

void fail_level() {
  egg_play_sound(RID_sound_lose);
  egg_play_song(0,0,1);
  g.mourntime=2.000;
}

/* Choose a different universe.
 * This really ought to be a modal, but for now just toggle thru them.
 */
 
void begin_universe_selection() {
  egg_play_sound(RID_sound_universe);
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

/* Explode.
 */
 
void explode(double x,double y) {
  egg_play_sound(RID_sound_explode);
  const double radius=0.888;
  int cola=(int)(x-radius); if (cola<0) cola=0;
  int colz=(int)(x+radius); if (colz>=NS_sys_mapw) colz=NS_sys_mapw-1;
  int rowa=(int)(y-radius); if (rowa<0) rowa=0;
  int rowz=(int)(y+radius); if (rowz>=NS_sys_maph) rowz=NS_sys_maph-1;
  uint8_t *cellrow=g.map+rowa*NS_sys_mapw+cola;
  int row=rowa; for (;row<=rowz;row++,cellrow+=NS_sys_mapw) {
    uint8_t *cellp=cellrow;
    int col=cola; for (;col<=colz;col++,cellp++) {
      uint8_t tileid=*cellp;
      if (tileid<0x80) tileid=(tileid&0x37)|((g.universe&2)<<5)|((g.universe&1)<<3);
      uint8_t physics=g.physics[tileid];
      if (physics==NS_physics_fragile) {
        *cellp=(tileid&0x48);
        g.bg_dirty=1;
      }
    }
  }
  struct sprite *deco=sprite_new(&sprite_type_explode,x,y);
}

/* Spells.
 */
 
void cast_spell() {
  char norm[SPELL_LIMIT];
  int srcp=g.spellp;
  int i=0;
  for (;i<SPELL_LIMIT;i++,srcp++) {
    if (srcp>=SPELL_LIMIT) srcp=0;
    switch (g.spellv[srcp]) {
      case EGG_BTN_UP: norm[i]='u'; break;
      case EGG_BTN_DOWN: norm[i]='d'; break;
      case EGG_BTN_LEFT: norm[i]='l'; break;
      case EGG_BTN_RIGHT: norm[i]='r'; break;
      case EGG_BTN_SOUTH: norm[i]='a'; break;
      case EGG_BTN_WEST: norm[i]='b'; break;
      default: norm[i]=' '; break;
    }
  }
  //fprintf(stderr,"Cast spell: %.*s\n",SPELL_LIMIT,norm);
  #define MATCH(src) (!memcmp(norm+SPELL_LIMIT-sizeof(src)+1,src,sizeof(src)-1))
  if (MATCH("ddd")) { reset_level(); return; }
  #undef MATCH
}

/* Soulballs.
 */
 
void generate_soulballs(double x,double y,int c) {
  struct sprite *sprite=sprite_new(&sprite_type_soulballs,x,y);
  if (!sprite) return;
  sprite->iv[0]=c;
}
