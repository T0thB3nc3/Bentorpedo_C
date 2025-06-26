#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "src/game.h"

// Egyszerű assert makró
#define ASSERT(cond, msg) do { \
    if (!(cond)) { printf("HIBA: %s\n", msg); return 1; } \
    else { printf("OK: %s\n", msg); } \
} while(0)

// Segédfüggvény: minden cella üres-e
int is_board_empty(GameBoard *board) {
    for (int i = 0; i < board->height; i++)
        for (int j = 0; j < board->width; j++)
            if (board->cells[i][j] != CELL_EMPTY)
                return 0;
    return 1;
}

int main() {
    printf("Bentorpedo automatikus tesztelés\n");

    // 1. Játék inicializálás
    GameState game;
    int width = 8, height = 8, ships = 4;
    init_game(&game, width, height, ships, AI_MEDIUM);
    ASSERT(game.player.width == width && game.player.height == height, "Pályaméret helyes inicializálás");
    ASSERT(game.player.ship_count == ships, "Hajók száma helyes inicializálás");
    ASSERT(game.ai.ship_count == ships, "AI hajók száma helyes inicializálás");

    // 2. Hajók elhelyezése
    int player_ships = 0, ai_ships = 0;
    for (int s = 0; s < ships; s++) {
        player_ships += game.player.ships[s].size;
        ai_ships += game.ai.ships[s].size;
    }
    int player_cells = 0, ai_cells = 0;
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++) {
            if (game.player.cells[i][j] == CELL_SHIP) player_cells++;
            if (game.ai.cells[i][j] == CELL_SHIP) ai_cells++;
        }
    ASSERT(player_ships == player_cells, "Játékos hajók helyesen a táblán");
    ASSERT(ai_ships == ai_cells, "AI hajók helyesen a táblán");

    // 3. Lövés hajóra és mellé
    int x = game.ai.ships[0].x, y = game.ai.ships[0].y;
    handle_player_input(&game, ' ', x, y);
    ASSERT(game.ai.cells[y][x] == CELL_HIT, "Lövés hajóra: találat");
    // Mellé lövés keresése
    int miss_x = -1, miss_y = -1;
    for (int i = 0; i < height && miss_x == -1; i++)
        for (int j = 0; j < width; j++)
            if (game.ai.cells[i][j] == CELL_EMPTY) { miss_x = j; miss_y = i; break; }
    handle_player_input(&game, ' ', miss_x, miss_y);
    ASSERT(game.ai.cells[miss_y][miss_x] == CELL_MISS, "Lövés üres mezőre: mellé");

    // 4. Speciális támadás (legalább egy hajó kilövése után)
    // Előfeltétel: legalább egy hajó már sérült
    int orig_remaining = game.ai.ships_remaining;
    handle_player_input(&game, 'a', x, y); // Speciális támadás ugyanarra a mezőre
    ASSERT(game.ai.ships_remaining <= orig_remaining, "Speciális támadás végrehajtható");

    // 5. Speciális védekezés (legalább egy hajó kilövése után)
    // Előfeltétel: legalább egy találat a játékos tábláján
    int px = game.player.ships[0].x, py = game.player.ships[0].y;
    game.player.cells[py][px] = CELL_HIT;
    handle_player_input(&game, 'd', px, py);
    ASSERT(game.player.cells[py][px] == CELL_SHIP, "Speciális védekezés visszaállít egy találatot");

    // 6. Játék vége detektálás
    game.ai.ships_remaining = 0;
    ASSERT(check_game_over(&game) == 1, "Játékos győzelem detektálása");
    game.ai.ships_remaining = 1;
    game.player.ships_remaining = 0;
    ASSERT(check_game_over(&game) == -1, "AI győzelem detektálása");

    free_game(&game);
    printf("Minden teszt sikeresen lefutott!\n");
    return 0;
}
