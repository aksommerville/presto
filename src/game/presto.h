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
#include "menu.h"

#define FBW 320
#define FBH 176

#define SPELL_LIMIT 8
#define SPELL_MIN_INTERVAL 0.500
#define TREADLE_LIMIT 8
#define LOCK_LIMIT 16

extern struct g {
  void *rom;
  int romc;
  struct graf graf;
  struct texcache texcache;
  struct font *font;
  int pvinput;
  
  /* I don't want to think too much about this.
   * If either of these is not null, it's the active mode.
   * Both null, the game is running.
   */
  struct hello *hello;
  struct gameover *gameover;
  
  int universe; // NS_uv_*
  uint8_t physics[256];
  uint8_t map[NS_sys_mapw*NS_sys_maph];
  int mapid;
  double celebration; // Counts down during each level's victory denouement.
  double mourntime; // ''
  int exploded; // For limiting sound effect.
  double besttime;
  double gametime;
  int deathc;
  
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
  
  struct treadle {
    uint8_t x,y;
    uint8_t flag;
    uint8_t state;
  } treadlev[TREADLE_LIMIT];
  int treadlec;
  
  struct lock {
    uint8_t x,y;
    uint8_t flag;
    uint8_t state;
    uint8_t direction; // 0=true->true, 1=true->false */
  } lockv[LOCK_LIMIT];
  int lockc;
} g;

int begin_level(int id);
void reset_level();
void win_level();
void fail_level();
void begin_universe_selection();
void set_universe(int uv);
void explode(double x,double y);
void cast_spell();
void generate_soulballs(double x,double y,int c);
void check_treadles();
void check_high_score();
void load_high_score();

#endif
