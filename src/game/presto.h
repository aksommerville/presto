#ifndef PRESTO_H
#define PRESTO_H

#include "egg/egg.h"
#include "opt/stdlib/egg-stdlib.h"
#include "opt/graf/graf.h"
#include "opt/text/text.h"
#include "opt/rom/rom.h"
#include "egg_rom_toc.h"
#include "shared_symbols.h"
#include "sprite.h"

#define FBW 320
#define FBH 176

extern struct g {
  void *rom;
  int romc;
  struct graf graf;
  struct texcache texcache;
  struct font *font;
  int pvinput;
  
  int universe; // NS_uv_*
  uint8_t physics[256];
  uint8_t map[NS_sys_mapw*NS_sys_maph];
  
  int bg_dirty;
  int texid_bg;
  int texid_tiles;
  
  struct sprite spritev[SPRITE_LIMIT];
  int spritec,spritea;
} g;

void begin_level(int id);
void begin_universe_selection();
void set_universe(int uv);

#endif
