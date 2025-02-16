#include "presto.h"

#define LABEL_LIMIT 4

struct gameover {
  int submitted;
  struct label {
    int texid;
    int x,y,w,h;
  } labelv[LABEL_LIMIT];
  int labelc;
};

/* Delete.
 */
 
void gameover_del(struct gameover *gameover) {
  if (!gameover) return;
  struct label *label=gameover->labelv;
  int i=gameover->labelc;
  for (;i-->0;label++) egg_texture_del(label->texid);
  free(gameover);
}

/* Add label to list.
 */
 
static struct label *gameover_add_label(struct gameover *gameover,const char *src,int srcc) {
  if (gameover->labelc>=LABEL_LIMIT) return 0;
  struct label *label=gameover->labelv+gameover->labelc++;
  memset(label,0,sizeof(struct label));
  label->texid=font_tex_oneline(g.font,src,srcc,FBW,0xffffffff);
  egg_texture_get_status(&label->w,&label->h,label->texid);
  return label;
}

/* Represnt time.
 */
 
static int time_repr(char *dst,int dsta,double src) {
  if (dsta<9) return 0;
  int ms=(int)(src*1000.0);
  if (ms<0) ms=0;
  int sec=ms/1000; ms%=1000;
  int min=sec/60; sec%=60;
  if (min>99) { min=99; sec=99; ms=999; }
  if (min>=10) {
    dst[0]='0'+min/10;
    dst[1]='0'+min%10;
    dst[2]=':';
    dst[3]='0'+sec/10;
    dst[4]='0'+sec%10;
    dst[5]='.';
    dst[6]='0'+(ms/100)%10;
    dst[7]='0'+(ms/10)%10;
    dst[8]='0'+ms%10;
    return 9;
  } else {
    dst[0]='0'+min%10;
    dst[1]=':';
    dst[2]='0'+sec/10;
    dst[3]='0'+sec%10;
    dst[4]='.';
    dst[5]='0'+(ms/100)%10;
    dst[6]='0'+(ms/10)%10;
    dst[7]='0'+ms%10;
    return 8;
  }
}

/* Init.
 */
 
static int gameover_init(struct gameover *gameover) {
  struct label *label;
  char text[256],v[16];
  int textc=0,htotal=0;
  struct strings_insertion ins;
  
  textc=strings_format(text,sizeof(text),1,10,&ins,1);
  if ((textc<0)||(textc>sizeof(text))) return -1;
  if (!(label=gameover_add_label(gameover,text,textc))) return -1;
  htotal+=label->h;
  
  ins.mode='i';
  ins.i=g.deathc;
  textc=strings_format(text,sizeof(text),1,11,&ins,1);
  if ((textc<0)||(textc>sizeof(text))) return -1;
  if (!(label=gameover_add_label(gameover,text,textc))) return -1;
  htotal+=label->h;
  
  ins.mode='s';
  ins.s.v=v;
  ins.s.c=time_repr(v,sizeof(v),g.gametime);
  textc=strings_format(text,sizeof(text),1,12,&ins,1);
  if ((textc<0)||(textc>sizeof(text))) return -1;
  if (!(label=gameover_add_label(gameover,text,textc))) return -1;
  htotal+=label->h;
  
  ins.mode='s';
  ins.s.v=v;
  ins.s.c=time_repr(v,sizeof(v),g.besttime);
  textc=strings_format(text,sizeof(text),1,13,&ins,1);
  if ((textc<0)||(textc>sizeof(text))) return -1;
  if (!(label=gameover_add_label(gameover,text,textc))) return -1;
  htotal+=label->h;
  
  int y=(FBH>>1)-(htotal>>1);
  int i=gameover->labelc;
  for (label=gameover->labelv;i-->0;label++) {
    label->x=(FBW>>1)-(label->w>>1);
    label->y=y;
    y+=label->h;
  }
  
  return 0;
}

/* New.
 */
 
struct gameover *gameover_new() {
  struct gameover *gameover=calloc(1,sizeof(struct gameover));
  if (!gameover) return 0;
  if (gameover_init(gameover)<0) {
    gameover_del(gameover);
    return 0;
  }
  egg_play_song(RID_song_all_four_of_us,0,1);
  return gameover;
}

/* Input.
 */
 
void gameover_input(struct gameover *gameover,int input,int pvinput) {
  if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) {
    gameover->submitted=1;
  }
}

/* Update.
 */
 
int gameover_update(struct gameover *gameover,double elapsed) {
  if (gameover->submitted) return 0;
  return 1;
}

/* Render.
 */
 
void gameover_render(struct gameover *gameover) {
  graf_draw_rect(&g.graf,0,0,FBW,FBH,0x402020ff);
  struct label *label=gameover->labelv;
  int i=gameover->labelc;
  for (;i-->0;label++) {
    graf_draw_decal(&g.graf,label->texid,label->x,label->y,0,0,label->w,label->h,0);
  }
}
