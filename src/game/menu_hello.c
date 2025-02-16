#include "presto.h"

#define OPTION_LIMIT 8

#define OPTIONID_TITLE 2
#define OPTIONID_PLAY 3
#define OPTIONID_INPUT 4
#define OPTIONID_QUIT 5
#define OPTIONID_MSG 6 /* Multiple strings, changing over time. */

#define MSG_TIME 2.500

static const int msg_stringixv[]={9,6,7,8};

struct hello {
  int submitted;
  struct option {
    int selectable;
    int optionid; // index in strings:1
    int texid;
    int w,h;
    int x,y;
  } optionv[OPTION_LIMIT];
  int optionc;
  int optionp;
  double msgclock;
  int msgixp;
};

/* Delete.
 */
 
void hello_del(struct hello *hello) {
  if (!hello) return;
  struct option *option=hello->optionv;
  int i=hello->optionc;
  for (;i-->0;option++) {
    egg_texture_del(option->texid);
  }
  free(hello);
}

/* Add option.
 */
 
static struct option *hello_add_option(struct hello *hello,int optionid) {
  if (hello->optionc>=OPTION_LIMIT) return 0;
  struct option *option=hello->optionv+hello->optionc++;
  memset(option,0,sizeof(struct option));
  if (optionid) {
    option->optionid=optionid;
    option->texid=font_texres_oneline(g.font,1,optionid,FBW,0xffffffff);
    egg_texture_get_status(&option->w,&option->h,option->texid);
    option->x=(FBW>>1)-(option->w>>1);
  }
  return option;
}

/* Init.
 */
 
static int hello_init(struct hello *hello) {
  struct option *option;
  
  if (!(option=hello_add_option(hello,OPTIONID_TITLE))) return -1;
  option->y=(FBH>>2)-(option->h>>1);
  
  int dsty=FBH>>1;
  int ystride=15;
  hello->optionp=hello->optionc;
  if (!(option=hello_add_option(hello,OPTIONID_PLAY))) return -1;
  option->selectable=1;
  option->y=dsty;
  dsty+=ystride;
  
  if (!(option=hello_add_option(hello,OPTIONID_INPUT))) return -1;
  option->selectable=1;
  option->y=dsty;
  dsty+=ystride;
  
  if (!(option=hello_add_option(hello,OPTIONID_QUIT))) return -1;
  option->selectable=1;
  option->y=dsty;
  dsty+=ystride;
  
  if (!(option=hello_add_option(hello,0))) return -1;
  option->optionid=OPTIONID_MSG;
  option->y=FBH;
  
  return 0;
}

/* New.
 */
 
struct hello *hello_new() {
  struct hello *hello=calloc(1,sizeof(struct hello));
  if (!hello) return 0;
  if (hello_init(hello)<0) {
    hello_del(hello);
    return 0;
  }
  egg_play_song(RID_song_all_four_of_us,0,1);
  return hello;
}

/* Activate selected option.
 */
 
static void hello_activate(struct hello *hello) {
  if ((hello->optionp<0)||(hello->optionp>=hello->optionc)) return;
  struct option *option=hello->optionv+hello->optionp;
  switch (option->optionid) {
    case OPTIONID_PLAY: hello->submitted=1; break;
    case OPTIONID_INPUT: egg_input_configure(); break;
    case OPTIONID_QUIT: egg_terminate(0); break;
  }
}

/* Move cursor.
 */
 
static void hello_move(struct hello *hello,int d) {
  egg_play_sound(RID_sound_uimotion);
  int panic=hello->optionc;
  while (panic-->0) {
    hello->optionp+=d;
    if (hello->optionp<0) hello->optionp=hello->optionc-1;
    else if (hello->optionp>=hello->optionc) hello->optionp=0;
    if (hello->optionv[hello->optionp].selectable) return;
  }
}

/* Input.
 */
 
void hello_input(struct hello *hello,int input,int pvinput) {
  if ((input&EGG_BTN_UP)&&!(pvinput&EGG_BTN_UP)) hello_move(hello,-1);
  if ((input&EGG_BTN_DOWN)&&!(pvinput&EGG_BTN_DOWN)) hello_move(hello,1);
  if ((input&EGG_BTN_SOUTH)&&!(pvinput&EGG_BTN_SOUTH)) hello_activate(hello);
}

/* Prepare the next passive message.
 */
 
static void hello_next_message(struct hello *hello) {
  struct option *option=0;
  struct option *q=hello->optionv;
  int i=hello->optionc;
  for (;i-->0;q++) {
    if (q->optionid==OPTIONID_MSG) {
      option=q;
      break;
    }
  }
  if (!option) return;
  
  int msgc=sizeof(msg_stringixv)/sizeof(msg_stringixv[0]);
  if (msgc<1) return;
  if ((hello->msgixp<0)||(hello->msgixp>=msgc)) hello->msgixp=0;
  int ix=msg_stringixv[hello->msgixp++];
  int texid=-1;
  
  // 9 is special: It shows the best time. Skip if there isn't a best recorded.
  if (ix==9) {
    if (g.besttime<=0.0) {
      hello->msgclock=0.0;
      return;
    }
    int ms=(int)(g.besttime*1000.0);
    if (ms<0) ms=0;
    int s=ms/1000; ms%=1000;
    int min=s/60; s%=60;
    if (min<100) {
      char bt[9];
      int btc=0;
      if (min>=10) bt[btc++]='0'+min/10;
      bt[btc++]='0'+min%10;
      bt[btc++]=':';
      bt[btc++]='0'+s/10;
      bt[btc++]='0'+s%10;
      bt[btc++]='.';
      bt[btc++]='0'+(ms/100)%10;
      bt[btc++]='0'+(ms/10)%10;
      bt[btc++]='0'+ms%10;
      struct strings_insertion ins={'s',.s={.v=bt,.c=btc}};
      char text[256];
      int textc=strings_format(text,sizeof(text),1,9,&ins,1);
      if ((textc>0)&&(textc<=sizeof(text))) {
        texid=font_tex_oneline(g.font,text,textc,FBW,0xffffffff);
      }
    }
    
  } else {
    texid=font_texres_oneline(g.font,1,ix,FBW,0xffffffff);
  }
  if (texid<1) return;
  egg_texture_del(option->texid);
  option->texid=texid;
  egg_texture_get_status(&option->w,&option->h,texid);
  option->x=(FBW>>1)-(option->w>>1);
  option->y=FBH-2-option->h;
}

/* Update.
 */
 
int hello_update(struct hello *hello,double elapsed) {
  if (hello->submitted) return 0;
  if ((hello->msgclock-=elapsed)<=0.0) {
    hello->msgclock+=MSG_TIME;
    hello_next_message(hello);
  }
  return 1;
}

/* Render.
 */
 
void hello_render(struct hello *hello) {
  graf_draw_rect(&g.graf,0,0,FBW,FBH,0x203040ff);
  if ((hello->optionp>=0)&&(hello->optionp<hello->optionc)) {
    struct option *option=hello->optionv+hello->optionp;
    graf_draw_rect(&g.graf,option->x-2,option->y-2,option->w+4,option->h+3,0x405080ff);
  }
  struct option *option=hello->optionv;
  int i=hello->optionc;
  for (;i-->0;option++) {
    graf_draw_decal(&g.graf,option->texid,option->x,option->y,0,0,option->w,option->h,0);
  }
}
