#ifndef GAME_H
#define GAME_H

#include <ncurses.h>

#define MAX_SHIPS 10
#define MAX_BOARD_SIZE 20

typedef enum {
    CELL_EMPTY,
    CELL_SHIP,
    CELL_HIT,
    CELL_MISS
} CellState;

typedef struct {
    int x, y;
    int size;
    int horizontal;
    int hits;
} Ship;

typedef struct {
    int width;
    int height;
    CellState **cells;
    Ship *ships;
    int ship_count;
    int ships_remaining;
} GameBoard;

typedef enum {
    AI_EASY,
    AI_MEDIUM,
    AI_HARD,
    AI_ADAPTIVE
} AILevel;

typedef struct {
    GameBoard player;
    GameBoard ai;
    AILevel ai_level;
    int player_turn;
} GameState;

void init_game(GameState *game, int width, int height, int ship_count, AILevel ai_level);
void free_game(GameState *game);
void display_boards(GameState *game);
void handle_player_input(GameState *game, int input, int cursor_x, int cursor_y);
void ai_move(GameState *game);
int check_game_over(GameState *game);
void place_ships(GameBoard *board, int ship_count, int is_ai);
void special_attack(GameState *game, int x, int y);
void special_defense(GameState *game);

#endif