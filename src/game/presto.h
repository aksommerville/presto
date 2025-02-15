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

#define SPELL_LIMIT 8
#define SPELL_MIN_INTERVAL 0.500

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
  int mapid;
  
  int bg_dirty;
  int texid_bg;
  int texid_tiles;
  
  struct sprite spritev[SPRITE_LIMIT];
  int spritec,spritea;
  
  /* Spells are probably only going to be "ddd" to restart.
   * But it's generalized and we can add spells arbitrarily.
   */
  uint16_t spellv[SPELL_LIMIT];
  int spellp;
  double spellclock;
} g;

void begin_level(int id);
void reset_level();
void begin_universe_selection();
void set_universe(int uv);
void explode(double x,double y);
void cast_spell();

#endif
