/* menu.h
 * Hello and gameover splashes, both unique things, no general UI abstractions.
 */
 
#ifndef MENU_H
#define MENU_H

struct hello;
struct gameover;

void hello_del(struct hello *hello);
struct hello *hello_new();
void hello_input(struct hello *hello,int input,int pvinput);
int hello_update(struct hello *hello,double elapsed); // 0 to launch game
void hello_render(struct hello *hello);

void gameover_del(struct gameover *gameover);
struct gameover *gameover_new();
void gameover_input(struct gameover *gameover,int input,int pvinput);
int gameover_update(struct gameover *gameover,double elapsed); // 0 to return to hello
void gameover_render(struct gameover *gameover);

#endif
