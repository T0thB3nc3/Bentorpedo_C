#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <omp.h>
#include "game.h"

void allocate_board(GameBoard *board, int width, int height, int ship_count) {
    board->width = width;
    board->height = height;
    board->cells = malloc(height * sizeof(CellState *));
    for (int i = 0; i < height; i++) {
        board->cells[i] = malloc(width * sizeof(CellState));
        for (int j = 0; j < width; j++)
            board->cells[i][j] = CELL_EMPTY;
    }
    board->ships = malloc(ship_count * sizeof(Ship));
    board->ship_count = ship_count;
    board->ships_remaining = ship_count;
}

void free_board(GameBoard *board) {
    for (int i = 0; i < board->height; i++)
        free(board->cells[i]);
    free(board->cells);
    free(board->ships);
}

void place_ships(GameBoard *board, int ship_count, int is_ai) {
    for (int s = 0; s < ship_count; s++) {
        int size = 2 + s % 3;
        int placed = 0;
        while (!placed) {
            int x = rand() % board->width;
            int y = rand() % board->height;
            int horizontal = rand() % 2;
            int fits = 1;
            for (int i = 0; i < size; i++) {
                int nx = x + (horizontal ? i : 0);
                int ny = y + (horizontal ? 0 : i);
                if (nx >= board->width || ny >= board->height || board->cells[ny][nx] != CELL_EMPTY) {
                    fits = 0;
                    break;
                }
            }
            if (fits) {
                for (int i = 0; i < size; i++) {
                    int nx = x + (horizontal ? i : 0);
                    int ny = y + (horizontal ? 0 : i);
                    board->cells[ny][nx] = CELL_SHIP;
                }
                board->ships[s] = (Ship){x, y, size, horizontal, 0};
                placed = 1;
            }
        }
    }
}

void init_game(GameState *game, int width, int height, int ship_count, AILevel ai_level) {
    allocate_board(&game->player, width, height, ship_count);
    allocate_board(&game->ai, width, height, ship_count);
    place_ships(&game->player, ship_count, 0);
    place_ships(&game->ai, ship_count, 1);
    game->ai_level = ai_level;
    game->player_turn = 1;
}

void free_game(GameState *game) {
    free_board(&game->player);
    free_board(&game->ai);
}

void display_boards(GameState *game) {
    mvprintw(0, 0, "Your Board:");
    mvprintw(0, game->player.width + 5, "Enemy Board:");
    for (int i = 0; i < game->player.height; i++) {
        for (int j = 0; j < game->player.width; j++) {
            char c = '.';
            if (game->player.cells[i][j] == CELL_SHIP) c = 'S';
            if (game->player.cells[i][j] == CELL_HIT) c = 'X';
            if (game->player.cells[i][j] == CELL_MISS) c = 'o';
            mvprintw(i + 1, j, "%c", c);

            char ec = '.';
            if (game->ai.cells[i][j] == CELL_HIT) ec = 'X';
            if (game->ai.cells[i][j] == CELL_MISS) ec = 'o';
            mvprintw(i + 1, j + game->player.width + 5, "%c", ec);
        }
    }
    refresh();
}

static int player_last_sunk = 0;

void handle_player_input(GameState *game, int input, int cursor_x, int cursor_y) {
    switch (input) {
        case '\n':
        case ' ':
            if (game->ai.cells[cursor_y][cursor_x] == CELL_EMPTY || game->ai.cells[cursor_y][cursor_x] == CELL_SHIP) {
                int hit = 0;
                int sunk_before = game->ai.ships_remaining;
                #pragma omp parallel for reduction(+:hit)
                for (int s = 0; s < game->ai.ship_count; s++) {
                    Ship *ship = &game->ai.ships[s];
                    for (int i = 0; i < ship->size; i++) {
                        int sx = ship->x + (ship->horizontal ? i : 0);
                        int sy = ship->y + (ship->horizontal ? 0 : i);
                        if (sx == cursor_x && sy == cursor_y && game->ai.cells[sy][sx] == CELL_SHIP) {
                            hit++;
                            game->ai.cells[sy][sx] = CELL_HIT;
                            ship->hits++;
                            if (ship->hits == ship->size)
                                game->ai.ships_remaining--;
                        }
                    }
                }
                if (!hit && game->ai.cells[cursor_y][cursor_x] == CELL_EMPTY)
                    game->ai.cells[cursor_y][cursor_x] = CELL_MISS;
                if (game->ai.ships_remaining < sunk_before) {
                    player_last_sunk++;
                }
            }
            break;
        case 'a': {
            if (player_last_sunk > 0) {
                int sunk_before = game->ai.ships_remaining;
                int prev_sunk = player_last_sunk;
                special_attack(game, cursor_x, cursor_y);
                if (game->ai.ships_remaining < sunk_before) {
                    player_last_sunk--;
                } else if (prev_sunk != player_last_sunk) {
                    player_last_sunk = prev_sunk;
                }
            }
            break;
        }
        case 'd': {
            if (player_last_sunk > 0) {
                int sunk_before = game->ai.ships_remaining;
                int prev_sunk = player_last_sunk;
                special_defense(game);
                if (game->ai.ships_remaining < sunk_before) {
                    player_last_sunk--;
                } else if (prev_sunk != player_last_sunk) {
                    player_last_sunk = prev_sunk;
                }
            }
            break;
        }
    }
}

typedef struct {
    int x, y;
} Point;

static int ai_find_next_target(GameBoard *board, int *last_hit_x, int *last_hit_y, int max_chain) {
    int dirs[4][2] = {{1,0},{-1,0},{0,1},{0,-1}};
    int x = *last_hit_x, y = *last_hit_y;
    for (int d = 0; d < 4 && max_chain > 0; d++, max_chain--) {
        int nx = x + dirs[d][0], ny = y + dirs[d][1];
        if (nx >= 0 && nx < board->width && ny >= 0 && ny < board->height) {
            if (board->cells[ny][nx] == CELL_EMPTY || board->cells[ny][nx] == CELL_SHIP) {
                *last_hit_x = nx; *last_hit_y = ny;
                return 1;
            }
        }
    }
    return 0;
}

void ai_move(GameState *game) {
    static int last_hit_x = -1, last_hit_y = -1;
    static int player_hits = 0, ai_moves = 0;
    static int adapt_level = 1;
    static int player_shot_sum = 0, player_ship_sunk = 0;
    ai_moves++;

    int x = 0, y = 0, valid = 0;
    int max_chain = 1;
    if (game->ai_level == AI_EASY) max_chain = 1;
    else if (game->ai_level == AI_MEDIUM) max_chain = 2;
    else if (game->ai_level == AI_HARD) max_chain = 4;
    else if (game->ai_level == AI_ADAPTIVE) max_chain = adapt_level;

    if (last_hit_x != -1 && last_hit_y != -1) {
        int found = ai_find_next_target(&game->player, &last_hit_x, &last_hit_y, max_chain);
        if (found) {
            x = last_hit_x; y = last_hit_y;
            valid = 1;
        }
    }
    if (!valid) {
        int tries = 0;
        do {
            x = rand() % game->player.width;
            y = rand() % game->player.height;
            tries++;
        } while ((game->player.cells[y][x] == CELL_HIT || game->player.cells[y][x] == CELL_MISS) && tries < 100);
        last_hit_x = x; last_hit_y = y;
    }

    if (game->player.cells[y][x] == CELL_SHIP) {
        game->player.cells[y][x] = CELL_HIT;
        last_hit_x = x; last_hit_y = y;
        #pragma omp parallel for
        for (int s = 0; s < game->player.ship_count; s++) {
            Ship *ship = &game->player.ships[s];
            for (int i = 0; i < ship->size; i++) {
                int sx = ship->x + (ship->horizontal ? i : 0);
                int sy = ship->y + (ship->horizontal ? 0 : i);
                if (sx == x && sy == y)
                    ship->hits++;
            }
            if (game->ai_level == AI_ADAPTIVE && ship->hits == ship->size) {
                player_ship_sunk++;
                player_shot_sum += ai_moves;
                ai_moves = 0;
                double avg = player_ship_sunk ? (double)player_shot_sum / player_ship_sunk : 0;
                int total_size = 0;
                for (int ss = 0; ss < game->player.ship_count; ss++)
                    total_size += game->player.ships[ss].size;
                double ideal = (double)total_size / game->player.ship_count;
                if (avg < ideal * 0.9 && adapt_level < 4) adapt_level++;
                else if (avg > ideal * 1.2 && adapt_level > 1) adapt_level--;
            }
        }
        player_hits++;
    } else {
        game->player.cells[y][x] = CELL_MISS;
        last_hit_x = -1; last_hit_y = -1;
    }
    game->player_turn = 1;
}

void special_attack(GameState *game, int x, int y) {
    int w = game->ai.width, h = game->ai.height;
    #pragma omp parallel for collapse(2)
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nx = x + dx, ny = y + dy;
            if (nx >= 0 && nx < w && ny >= 0 && ny < h) {
                if (game->ai.cells[ny][nx] == CELL_SHIP) {
                    game->ai.cells[ny][nx] = CELL_HIT;
                    for (int s = 0; s < game->ai.ship_count; s++) {
                        Ship *ship = &game->ai.ships[s];
                        for (int i = 0; i < ship->size; i++) {
                            int sx = ship->x + (ship->horizontal ? i : 0);
                            int sy = ship->y + (ship->horizontal ? 0 : i);
                            if (sx == nx && sy == ny) {
                                #pragma omp atomic
                                ship->hits++;
                                if (ship->hits == ship->size) {
                                    #pragma omp atomic
                                    game->ai.ships_remaining--;
                                }
                            }
                        }
                    }
                } else if (game->ai.cells[ny][nx] == CELL_EMPTY) {
                    game->ai.cells[ny][nx] = CELL_MISS;
                }
            }
        }
    }
}

void special_defense(GameState *game) {
    int repaired = 0;
    int w = game->player.width, h = game->player.height;
    int attempts = 0, max_attempts = w * h * 2;
    while (repaired < 3 && attempts < max_attempts) {
        int x = rand() % w;
        int y = rand() % h;
        if (game->player.cells[y][x] == CELL_HIT) {
            game->player.cells[y][x] = CELL_SHIP;
            repaired++;
        }
        attempts++;
    }
}

int check_game_over(GameState *game) {
    if (game->player.ships_remaining == 0)
        return -1; // AI wins
    if (game->ai.ships_remaining == 0)
        return 1; // Player wins
    return 0; // Continue
}

