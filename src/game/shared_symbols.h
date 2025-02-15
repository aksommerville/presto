/* shared_symbols.h
 * Consumed by both the game and the tools.
 */

#ifndef SHARED_SYMBOLS_H
#define SHARED_SYMBOLS_H

#define NS_sys_tilesize 16
#define NS_sys_mapw 20
#define NS_sys_maph 11

#define CMD_map_image     0x20 /* u16:imageid */
#define CMD_map_sprite    0x40 /* u16:pos u16:spritetype */

#define CMD_sprite_image  0x20 /* u16:imageid */
#define CMD_sprite_tile   0x21 /* u8:tileid u8:xform */

#define NS_tilesheet_physics     1
#define NS_tilesheet_neighbors   0
#define NS_tilesheet_family      0
#define NS_tilesheet_weight      0

#define NS_physics_vacant 0
#define NS_physics_solid 1
#define NS_physics_goal 2
#define NS_physics_timemachine 3
#define NS_physics_fragile 4

#define NS_uv_pumpkin 0
#define NS_uv_balloon 1
#define NS_uv_bomb 2
#define NS_uv_hippopotamus 3

#define NS_spritetype_hero 1
#define NS_spritetype_pumpkin 2
#define NS_spritetype_balloon 3
#define NS_spritetype_bomb 4
#define NS_spritetype_hippopotamus 5
#define NS_spritetype_explode 6
#define FOR_EACH_spritetype \
  _(hero) \
  _(pumpkin) \
  _(balloon) \
  _(bomb) \
  _(hippopotamus) \
  _(explode)

#endif
