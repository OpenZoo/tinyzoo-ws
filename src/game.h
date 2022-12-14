#ifndef __GAME_H__
#define __GAME_H__

#include <stdint.h>
#include "gamevars.h"

extern int8_t viewport_x;
extern int8_t viewport_y;
extern uint8_t viewport_focus_stat;
extern bool viewport_focus_locked;

#ifdef HACK_HIDE_STATUSBAR
#define viewport_full_board true
#else
extern bool viewport_full_board;
#endif

#define VIEWPORT_MIN_X 1
#define VIEWPORT_MAX_X (BOARD_WIDTH - VIEWPORT_WIDTH + 1)
#define VIEWPORT_MIN_Y 1
#define VIEWPORT_MAX_Y (BOARD_HEIGHT - VIEWPORT_HEIGHT + (viewport_full_board ? 1 : 2))

extern const zoo_stat_t __far stat_template_default;
extern const int8_t __far neighbor_delta_x[4];
extern const int8_t __far neighbor_delta_y[4];
extern const int8_t __far diagonal_delta_x[8];
extern const int8_t __far diagonal_delta_y[8];

void wait_vbl_done(void);

// game.c (bank 0)

void center_viewport_on(int8_t x, int8_t y);
void viewport_reset(void);
bool viewport_request_player_focus(void);

void board_redraw(void);

void board_enter_stage1(void); // before transition
void board_enter_stage2(void); // during transition
void board_enter_stage3(void); // after transition
void board_change(uint8_t id);
void board_undraw_tile(uint8_t x, uint8_t y);
void board_draw_char(uint8_t x, uint8_t y, uint8_t chr, uint8_t col);
void board_draw_tile(uint8_t x, uint8_t y);
uint8_t get_stat_id_at(uint8_t x, uint8_t y);
void add_stat(uint8_t tx, uint8_t ty, uint8_t element, uint8_t color, uint8_t cycle, const zoo_stat_t __far* template);
void remove_stat(uint8_t stat_id);
void move_stat(uint8_t stat_id, uint8_t x, uint8_t y);
void damage_stat(uint8_t stat_id);
void board_damage_tile(uint8_t x, uint8_t y);
void calc_direction_rnd(int8_t *dx, int8_t *dy);
void calc_direction_seek(uint8_t x, uint8_t y, int8_t *dx, int8_t *dy);

void init_display_message(uint8_t time, bool visible);
void display_message(uint8_t time, const char __far* line1, const char __far* line2, const char __far* line3); // bank 3 only

// game_sidebar.c (bank 0)

void game_update_sidebar_all(void);
void game_update_sidebar_health(void);
void game_update_sidebar_ammo(void);
void game_update_sidebar_gems_time(void);
void game_update_sidebar_torches(void);
void game_update_sidebar_score(void);
void game_update_sidebar_keys(void);

// game_bank1.c (bank 1)

void board_attack(uint8_t stat_id, uint8_t x, uint8_t y);
void game_play_loop(bool board_changed);

// game_bank2.c (bank 2)

void board_create(bool full_create);
void world_create(void);
void scroll_viewport_to(uint8_t vx, uint8_t vy, bool force_redraw);
void game_scrolling_view(void);
void move_stat_scroll_focused(uint8_t stat_id, uint8_t old_x, uint8_t old_y, uint8_t new_x, uint8_t new_y, bool force);
bool board_shoot(uint8_t element, uint8_t x, uint8_t y, int8_t dx, int8_t dy, uint8_t source);

// game_menus.c (bank 2)

void game_menu_act_enter_world(uint8_t world_id, bool new_game, bool first_launch);
bool game_pause_menu(void);

#endif
