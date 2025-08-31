#include "presto.h"

struct g g={0};

void egg_client_quit(int status) {
}

static void load_tilesheet(const void *src,int srcc) {
  struct rom_tilesheet_reader reader;
  if (rom_tilesheet_reader_init(&reader,src,srcc)<0) return;
  struct rom_tilesheet_entry entry;
  while (rom_tilesheet_reader_next(&entry,&reader)>0) {
    if (entry.tableid==NS_tilesheet_physics) {
      memcpy(g.physics+entry.tileid,entry.v,entry.c);
    }
  }
}

int egg_client_init() {

  int fbw=0,fbh=0;
  egg_texture_get_status(&fbw,&fbh,1);
  if ((fbw!=FBW)||(fbh!=FBH)) return -1;
  
  if ((g.romc=egg_get_rom(0,0))<=0) return -1;
  if (!(g.rom=malloc(g.romc))) return -1;
  if (egg_get_rom(g.rom,g.romc)!=g.romc) return -1;
  strings_set_rom(g.rom,g.romc);
  
  if (!(g.font=font_new())) return -1;
  if (font_add_image_resource(g.font,0x0020,RID_image_font9_0020)<0) return -1;
  
  if (egg_texture_load_image(g.texid_tiles=egg_texture_new(),RID_image_tiles)<0) return -1;
  if (egg_texture_load_raw(g.texid_bg=egg_texture_new(),FBW,FBH,FBW<<2,0,0)<0) return -1;
  
  struct rom_reader reader;
  if (rom_reader_init(&reader,g.rom,g.romc)<0) return -1;
  struct rom_res *res;
  while (res=rom_reader_next(&reader)) {
    switch (res->tid) {
      case EGG_TID_tilesheet: if (res->rid==RID_tilesheet_tiles) load_tilesheet(res->v,res->c); break;
    }
  }
  
  srand_auto();
  
  load_high_score();
  
  if (!(g.hello=hello_new())) return -1;
  
  return 0;
}

void on_up() {
  struct sprite *hero=sprites_any_of_type(&sprite_type_hero);
  if (!hero) return;
  sprite_hero_interact(hero);
}

void on_b() {
  struct sprite *hero=sprites_any_of_type(&sprite_type_hero);
  if (!hero) return;
  sprite_hero_pickup(hero);
}

void egg_client_update(double elapsed) {
  g.exploded=0;

  int input=egg_input_get_one(0);
  if (input!=g.pvinput) {
    if (!g.hello&&!g.gameover) {
      uint16_t freshbits=input&~g.pvinput;
      int mask=1,cast=0;
      for (;mask<=freshbits;mask<<=1) {
        if (mask&freshbits) {
          g.spellv[g.spellp]=mask;
          if (++(g.spellp)>=SPELL_LIMIT) g.spellp=0;
          cast=1;
        }
      }
      if (cast) {
        cast_spell();
        g.spellclock=SPELL_MIN_INTERVAL;
      }
    }
    if ((input&EGG_BTN_AUX3)&&!(g.pvinput&EGG_BTN_AUX3)) egg_terminate(0);
    if ((input&EGG_BTN_AUX2)&&!(g.pvinput&EGG_BTN_AUX2)) egg_terminate(0);
    if (g.hello) hello_input(g.hello,input,g.pvinput);
    else if (g.gameover) gameover_input(g.gameover,input,g.pvinput);
    else {
      if ((input&EGG_BTN_UP)&&!(g.pvinput&EGG_BTN_UP)) on_up();
      if ((input&EGG_BTN_WEST)&&!(g.pvinput&EGG_BTN_WEST)) on_b();
    }
    g.pvinput=input;
  }
  
  if (g.hello) {
    if (!hello_update(g.hello,elapsed)) {
      hello_del(g.hello);
      g.hello=0;
      g.gametime=0.0;
      g.deathc=0;
      begin_level(1);
    } else {
      return;
    }
  }
  if (g.gameover) {
    if (!gameover_update(g.gameover,elapsed)) {
      gameover_del(g.gameover);
      g.gameover=0;
      if (!(g.hello=hello_new())||!hello_update(g.hello,elapsed)) {
        egg_terminate(1);
        return;
      }
    }
  }
  
  if (!g.hello&&!g.gameover) {
    if (g.celebration>0.0) {
      if ((g.celebration-=elapsed)<=0.0) {
        if (begin_level(g.mapid+1)<0) {
          if (!(g.gameover=gameover_new())||!gameover_update(g.gameover,elapsed)) {
            egg_terminate(1);
          }
          check_high_score();
          return;
        }
      }
    }
    if (g.mourntime>0.0) {
      if ((g.mourntime-=elapsed)<=0.0) {
        g.deathc++;
        begin_level(g.mapid);
      }
    }
  
    if (g.spellclock>0.0) {
      if ((g.spellclock-=elapsed)<=0.0) {
        memset(g.spellv,0,sizeof(g.spellv));
      }
    }
    
    if ((g.mourntime<=0.0)&&(g.celebration<=0.0)) {
      g.gametime+=elapsed;
    }
  
    struct sprite *sprite=g.spritev;
    int i=g.spritec;
    for (;i-->0;sprite++) {
      if (sprite->defunct) continue;
      if (sprite->type->update) sprite->type->update(sprite,elapsed);
    }
  
    sprites_drop_defunct();
    check_treadles();
  }
}

void egg_client_render() {
  graf_reset(&g.graf);
  
  if (g.hello) {
    hello_render(g.hello);
  } else if (g.gameover) {
    gameover_render(g.gameover);
  } else {
  
    if (g.bg_dirty) {
      struct egg_draw_tile vtxv[NS_sys_mapw*NS_sys_maph];
      struct egg_draw_tile *vtx=vtxv;
      const uint8_t *src=g.map;
      int16_t y=NS_sys_tilesize>>1;
      int yi=NS_sys_maph;
      for (;yi-->0;y+=NS_sys_tilesize) {
        int16_t x=NS_sys_tilesize>>1;
        int xi=NS_sys_mapw;
        for (;xi-->0;x+=NS_sys_tilesize,vtx++,src++) {
          vtx->dstx=x;
          vtx->dsty=y;
          if (*src<0x80) vtx->tileid=((*src)&0x37)|((g.universe&2)<<5)|((g.universe&1)<<3);
          else vtx->tileid=*src;
          vtx->xform=0;
        }
      }
      egg_draw_tile(g.texid_bg,g.texid_tiles,vtxv,NS_sys_mapw*NS_sys_maph);
    }
  
    graf_draw_decal(&g.graf,g.texid_bg,0,0,0,0,FBW,FBH,0);
  
    struct sprite *sprite=g.spritev;
    int i=g.spritec;
    for (;i-->0;sprite++) {
      if (sprite->type->render) {
        sprite->type->render(sprite);
      } else {
        int16_t dstx=(int)(sprite->x*NS_sys_tilesize);
        int16_t dsty=(int)(sprite->y*NS_sys_tilesize);
        graf_draw_tile(&g.graf,g.texid_tiles,dstx,dsty,sprite->tileid,sprite->xform);
      }
    }
  }
  
  graf_flush(&g.graf);
}
