#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <time.h>
#include "game.h"

#define CLASSIC_WIDTH 10
#define CLASSIC_HEIGHT 10
#define CLASSIC_SHIPS 5
#define MAX_NAME_LEN 32
#define MAX_STATS 10

typedef struct {
    char name[MAX_NAME_LEN];
    int turns;
    time_t timestamp;
} StatEntry;

void draw_ascii_title_centered(WINDOW *win, int starty) {
    const char *lines[] = {
        "  ____  ______ _   _ _______ ____  _____  _____  ______ _____   ____  ",
        " |  _ \\|  ____| \\ | |__   __/ __ \\|  __ \\|  __ \\|  ____|  __ \\ / __ \\ ",
        " | |_) | |__  |  \\| |  | | | |  | | |__) | |__) | |__  | |  | | |  | |",
        " |  _ <|  __| | . ` |  | | | |  | |  _  /|  ___/|  __| | |  | | |  | |",
        " | |_) | |____| |\\  |  | | | |__| | | \\ \\| |    | |____| |__| | |__| |",
        " |____/|______|_| \\_|  |_|  \\____/|_|  \\_\\_|    |______|_____/ \\____/ ",
        "                                                                      "
    };
    int width = getmaxx(win);
    int n_lines = sizeof(lines) / sizeof(lines[0]);
    for (int i = 0; i < n_lines; i++) {
        int len = strlen(lines[i]);
        mvwprintw(win, starty + i, (width - len) / 2, "%s", lines[i]);
    }
}

int main_menu() {
    const char *options[] = {
        "Klasszikus jatek",
        "Egyeni jatek",
        "Statisztikak",
        "Kilepes"
    };
    int n_options = sizeof(options)/sizeof(options[0]);
    int highlight = 0, ch;
    int win_h = 18, win_w = 50;
    int scr_h, scr_w;
    getmaxyx(stdscr, scr_h, scr_w);
    win_h = scr_h * 3 / 4;
    win_w = scr_w * 2 / 3;
    if (win_h < 18) win_h = 18;
    if (win_w < 50) win_w = 50;
    int win_y = (scr_h - win_h) / 2;
    int win_x = (scr_w - win_w) / 2;
    WINDOW *menuwin = newwin(win_h, win_w, win_y, win_x);
    keypad(menuwin, TRUE);

    int border_pair = has_colors() ? 6 : 0;
    int title_pair = has_colors() ? 1 : 0;
    int highlight_pair = has_colors() ? 2 : 0;
    int normal_pair = has_colors() ? 4 : 0;

    while (1) {
        werase(menuwin);
        if (has_colors()) wattron(menuwin, COLOR_PAIR(border_pair));
        box(menuwin, 0, 0);
        if (has_colors()) wattroff(menuwin, COLOR_PAIR(border_pair));
        if (has_colors()) wattron(menuwin, COLOR_PAIR(title_pair));
        draw_ascii_title_centered(menuwin, 2);
        if (has_colors()) wattroff(menuwin, COLOR_PAIR(title_pair));
        int menu_start = 10;
        for (int i = 0; i < n_options; i++) {
            int len = strlen(options[i]);
            int x = (win_w - len) / 2;
            if (i == highlight) {
                if (has_colors()) wattron(menuwin, COLOR_PAIR(highlight_pair));
                wattron(menuwin, A_REVERSE);
            } else {
                if (has_colors()) wattron(menuwin, COLOR_PAIR(normal_pair));
            }
            mvwprintw(menuwin, menu_start + i * 2, x, "%s", options[i]);
            if (i == highlight) {
                wattroff(menuwin, A_REVERSE);
                if (has_colors()) wattroff(menuwin, COLOR_PAIR(highlight_pair));
            } else {
                if (has_colors()) wattroff(menuwin, COLOR_PAIR(normal_pair));
            }
        }
        char *author = "Keszitette: Toth Bence";
        char *copyright = "Copyright@2025";
        if (has_colors()) wattron(menuwin, COLOR_PAIR(1));
        mvwprintw(menuwin, win_h - 2, 2, "%s", author);
        mvwprintw(menuwin, win_h - 2, win_w - (int)strlen(copyright) - 2, "%s", copyright);
        if (has_colors()) wattroff(menuwin, COLOR_PAIR(1));

        if (has_colors()) wattron(menuwin, COLOR_PAIR(1));
        mvwprintw(menuwin, win_h - 3, (win_w - 38) / 2, "Nyilakkal valaszthatsz, ENTER: OK");
        if (has_colors()) wattroff(menuwin, COLOR_PAIR(1));
        wrefresh(menuwin);
        ch = wgetch(menuwin);
        switch (ch) {
            case KEY_UP: if (highlight > 0) highlight--; break;
            case KEY_DOWN: if (highlight < n_options-1) highlight++; break;
            case '\n': delwin(menuwin); return highlight;
        }
    }
}

void settings_menu(int *width, int *height, int *ships, int *ai_level) {
    const char *ai_names[] = {"Konnyu", "Kozepes", "Nehez", "Adaptiv"};
    int sel = 0, ch;
    int scr_h, scr_w;
    getmaxyx(stdscr, scr_h, scr_w);
    int win_h = scr_h * 3 / 4;
    int win_w = scr_w * 2 / 3;
    if (win_h < 16) win_h = 16;
    if (win_w < 44) win_w = 44;
    int win_y = (scr_h - win_h) / 2;
    int win_x = (scr_w - win_w) / 2;
    WINDOW *setwin = newwin(win_h, win_w, win_y, win_x);
    keypad(setwin, TRUE);

    int border_pair = has_colors() ? 6 : 0;
    int title_pair = has_colors() ? 1 : 0;
    int highlight_pair = has_colors() ? 2 : 0;
    int normal_pair = has_colors() ? 4 : 0;

    while (1) {
        werase(setwin);
        if (has_colors()) wattron(setwin, COLOR_PAIR(border_pair));
        box(setwin, 0, 0);
        if (has_colors()) wattroff(setwin, COLOR_PAIR(border_pair));
        char *title = "Jatek beallitasok";
        if (has_colors()) wattron(setwin, COLOR_PAIR(title_pair));
        mvwprintw(setwin, 2, (win_w-strlen(title))/2, "%s", title);
        if (has_colors()) wattroff(setwin, COLOR_PAIR(title_pair));
        int row = 5;
        for (int i = 0; i < 5; i++) {
            int y = row + i*2;
            int x = (win_w-28)/2;
            if (i == sel) {
                if (has_colors()) wattron(setwin, COLOR_PAIR(highlight_pair));
                wattron(setwin, A_REVERSE);
            } else {
                if (has_colors()) wattron(setwin, COLOR_PAIR(normal_pair));
            }
            if (i == 0) mvwprintw(setwin, y, x, "Palya szelesseg: %d", *width);
            if (i == 1) mvwprintw(setwin, y, x, "Palya magassag:  %d", *height);
            if (i == 2) mvwprintw(setwin, y, x, "Hajok szama:     %d", *ships);
            if (i == 3) mvwprintw(setwin, y, x, "MI szint:        %s", ai_names[*ai_level]);
            if (i == 4) mvwprintw(setwin, y, x, "Mentes es inditas");
            if (i == sel) {
                wattroff(setwin, A_REVERSE);
                if (has_colors()) wattroff(setwin, COLOR_PAIR(highlight_pair));
            } else {
                if (has_colors()) wattroff(setwin, COLOR_PAIR(normal_pair));
            }
        }
        char *help = "Nyil: valtas, ENTER: modosit/indit, ESC: vissza";
        if (has_colors()) wattron(setwin, COLOR_PAIR(1));
        mvwprintw(setwin, win_h-3, (win_w-strlen(help))/2, "%s", help);
        if (has_colors()) wattroff(setwin, COLOR_PAIR(1));
        wrefresh(setwin);
        ch = wgetch(setwin);
        if (ch == 27) { delwin(setwin); break; }
        switch (ch) {
            case KEY_UP: if (sel > 0) sel--; break;
            case KEY_DOWN: if (sel < 4) sel++; break;
            case KEY_LEFT:
                if (sel == 0 && *width > 5) (*width)--;
                if (sel == 1 && *height > 5) (*height)--;
                if (sel == 2 && *ships > 1) (*ships)--;
                if (sel == 3 && *ai_level > 0) (*ai_level)--;
                break;
            case KEY_RIGHT:
                if (sel == 0 && *width < MAX_BOARD_SIZE) (*width)++;
                if (sel == 1 && *height < MAX_BOARD_SIZE) (*height)++;
                if (sel == 2 && *ships < MAX_SHIPS) (*ships)++;
                if (sel == 3 && *ai_level < 3) (*ai_level)++;
                break;
            case '\n':
                if (sel == 4) { delwin(setwin); return; }
                break;
        }
    }
}

void wait_for_key() {
    int scr_h, scr_w;
    getmaxyx(stdscr, scr_h, scr_w);
    mvprintw(scr_h-2, (scr_w-36)/2, "Nyomj meg egy billentyut a folytatashoz...");
    getch();
}

void place_ships_menu(GameBoard *board) {
    int placed = 0, dir = 1;
    int x = 0, y = 0;
    int scr_h, scr_w;
    getmaxyx(stdscr, scr_h, scr_w);
    int margin = 4;
    int keret_y = margin;
    int keret_x = margin;
    int keret_h = scr_h - 2 * margin;
    int keret_w = scr_w - 2 * margin;
    int grid_y = keret_y + 3;
    int grid_x = keret_x + 8;
    int win_h = keret_h;
    int win_w = keret_w;
    WINDOW *pswin = newwin(win_h, win_w, keret_y, keret_x);
    keypad(pswin, TRUE);

    while (placed < board->ship_count) {
        werase(pswin);
        if (has_colors()) wattron(pswin, COLOR_PAIR(6));
        box(pswin, 0, 0);
        if (has_colors()) wattroff(pswin, COLOR_PAIR(6));
        mvwprintw(pswin, 1, 2, "Hajok elhelyezese: Nyilak - mozgas, r - fordit, ENTER - lerak, q - kilep");
        mvwprintw(pswin, 2, 2, "A hajok nem fedhetik egymast es nem loghatnak ki a palyarol!");

        for (int i = 0; i < board->height; i++) {
            for (int j = 0; j < board->width; j++) {
                char c = '.';
                int color = 1;
                if (board->cells[i][j] == CELL_SHIP) { c = 'H'; color = 4; }
                if (i == y && j == x) wattron(pswin, A_REVERSE);
                if (has_colors()) wattron(pswin, COLOR_PAIR(color));
                mvwprintw(pswin, grid_y + i, grid_x + j*2, "%c", c);
                if (has_colors()) wattroff(pswin, COLOR_PAIR(color));
                if (i == y && j == x) wattroff(pswin, A_REVERSE);
            }
        }
        int size = 2 + placed % 3;
        int fits = 1;
        for (int i = 0; i < size; i++) {
            int nx = x + (dir ? i : 0);
            int ny = y + (dir ? 0 : i);
            if (nx >= board->width || ny >= board->height || board->cells[ny][nx] == CELL_SHIP) fits = 0;
        }
        if (fits) wattron(pswin, A_BOLD);
        mvwprintw(pswin, grid_y + board->height + 2, 4, "Aktualis hajo merete: %d, iranya: %s", size, dir ? "vizszintes" : "fuggoleges");
        if (fits) wattroff(pswin, A_BOLD);

        mvwprintw(pswin, grid_y + board->height + 4, 4, "Hajok meretei: ");
        for (int s = 0; s < board->ship_count; s++) {
            int placed_now = (s < placed);
            if (placed_now && has_colors()) wattron(pswin, COLOR_PAIR(4));
            wprintw(pswin, "%d ", 2 + s % 3);
            if (placed_now && has_colors()) wattroff(pswin, COLOR_PAIR(4));
        }

        wrefresh(pswin);
        int ch = wgetch(pswin);
        if (ch == 'q') { delwin(pswin); endwin(); exit(0); }
        if (ch == 'r') dir = !dir;
        if (ch == KEY_UP && y > 0) y--;
        if (ch == KEY_DOWN && y < board->height-1) y++;
        if (ch == KEY_LEFT && x > 0) x--;
        if (ch == KEY_RIGHT && x < board->width-1) x++;
        if (ch == '\n' && fits) {
            for (int i = 0; i < size; i++) {
                int nx = x + (dir ? i : 0);
                int ny = y + (dir ? 0 : i);
                board->cells[ny][nx] = CELL_SHIP;
            }
            board->ships[placed] = (Ship){x, y, size, dir, 0};
            placed++;
        }
    }
    delwin(pswin);
}

void show_stats_menu();
void save_stat(int ai_level, int turns);

void save_stat(int ai_level, int turns) {
    char filename[32];
    if (ai_level == AI_EASY) strcpy(filename, "stats_easy.dat");
    else if (ai_level == AI_MEDIUM) strcpy(filename, "stats_medium.dat");
    else if (ai_level == AI_HARD) strcpy(filename, "stats_hard.dat");
    else return;

    StatEntry stats[MAX_STATS+1];
    int count = 0;
    FILE *f = fopen(filename, "rb");
    if (f) {
        count = fread(stats, sizeof(StatEntry), MAX_STATS, f);
        fclose(f);
    }
    timeout(-1);
    echo();
    curs_set(1);
    int scr_h, scr_w;
    getmaxyx(stdscr, scr_h, scr_w);
    char name[MAX_NAME_LEN] = "";
    int valid = 0;
    while (!valid) {
        clear();
        char *prompt = "Gratulalunk! Add meg a neved (max 31 karakter):";
        mvprintw(scr_h/2-2, (scr_w-strlen(prompt))/2, "%s", prompt);
        move(scr_h/2, (scr_w-20)/2);
        clrtoeol();
        int res = getnstr(name, MAX_NAME_LEN-1);
        if (res == ERR) continue;
        if (strlen(name) == 0) {
            char *warn1 = "Ures nevvel nem kerul be az eredmeny a statisztikaba.";
            char *warn2 = "ESC: kihagyas, ENTER: ujra probalod";
            mvprintw(scr_h/2+2, (scr_w-strlen(warn1))/2, "%s", warn1);
            mvprintw(scr_h/2+3, (scr_w-strlen(warn2))/2, "%s", warn2);
            int c;
            while (1) {
                c = getch();
                if (c == 27) {
                    noecho();
                    curs_set(0);
                    timeout(1000);
                    return;
                }
                if (c == '\n' || c == '\r') break;
            }
            continue;
        }
        valid = 1;
    }
    noecho();
    curs_set(0);
    StatEntry new_entry;
    strncpy(new_entry.name, name, MAX_NAME_LEN-1);
    new_entry.name[MAX_NAME_LEN-1] = 0;
    new_entry.turns = turns;
    new_entry.timestamp = time(NULL);
    int inserted = 0;
    for (int i = 0; i < count; i++) {
        if (!inserted && turns < stats[i].turns) {
            for (int j = count; j > i; j--)
                stats[j] = stats[j-1];
            stats[i] = new_entry;
            inserted = 1;
            if (count < MAX_STATS) count++;
            break;
        }
    }
    if (!inserted && count < MAX_STATS) {
        stats[count++] = new_entry;
    }
    if (!inserted && count == MAX_STATS && turns < stats[MAX_STATS-1].turns) {
        stats[MAX_STATS-1] = new_entry;
    }
    f = fopen(filename, "wb");
    if (f) {
        fwrite(stats, sizeof(StatEntry), count, f);
        fclose(f);
    }
    clear();
    char *msg = "Eredmenyed bekerult a statisztikaba!";
    mvprintw(scr_h/2, (scr_w-strlen(msg))/2, "%s", msg);
    char *cont = "ESC vagy ENTER: tovabblepes a statisztikakhoz...";
    mvprintw(scr_h/2+2, (scr_w-strlen(cont))/2, "%s", cont);
    int c;
    while (1) {
        c = getch();
        if (c == 27 || c == '\n' || c == '\r') break;
    }
    timeout(1000);
    show_stats_menu();
}

void show_stats_menu() {
    const char *levels[] = {"Konnyu", "Kozepes", "Nehez"};
    const char *files[] = {"stats_easy.dat", "stats_medium.dat", "stats_hard.dat"};
    int sel = 0, ch;
    int scr_h, scr_w;
    getmaxyx(stdscr, scr_h, scr_w);
    int win_h = scr_h * 3 / 4;
    int win_w = scr_w * 2 / 3;
    if (win_h < 18) win_h = 18;
    if (win_w < 50) win_w = 50;
    int win_y = (scr_h - win_h) / 2;
    int win_x = (scr_w - win_w) / 2;
    WINDOW *statwin = newwin(win_h, win_w, win_y, win_x);
    keypad(statwin, TRUE);
    int border_pair = has_colors() ? 6 : 0;
    int title_pair = has_colors() ? 1 : 0;
    int highlight_pair = has_colors() ? 2 : 0;
    int normal_pair = has_colors() ? 4 : 0;

    while (1) {
        werase(statwin);
        if (has_colors()) wattron(statwin, COLOR_PAIR(border_pair));
        box(statwin, 0, 0);
        if (has_colors()) wattroff(statwin, COLOR_PAIR(border_pair));
        char *title = "Statisztikak (TOP 10 gyozelem, nevvel, kor, datum)";
        if (has_colors()) wattron(statwin, COLOR_PAIR(title_pair));
        mvwprintw(statwin, 2, (win_w-strlen(title))/2, "%s", title);
        if (has_colors()) wattroff(statwin, COLOR_PAIR(title_pair));
        for (int l = 0; l < 3; l++) {
            int x = (win_w-40)/2 + l*15;
            if (l == sel) {
                if (has_colors()) wattron(statwin, COLOR_PAIR(highlight_pair));
                wattron(statwin, A_REVERSE);
            } else {
                if (has_colors()) wattron(statwin, COLOR_PAIR(normal_pair));
            }
            mvwprintw(statwin, 4, x, "%s", levels[l]);
            if (l == sel) {
                wattroff(statwin, A_REVERSE);
                if (has_colors()) wattroff(statwin, COLOR_PAIR(highlight_pair));
            } else {
                if (has_colors()) wattroff(statwin, COLOR_PAIR(normal_pair));
            }
        }
        char *header = "Nev                Kor   Datum";
        if (has_colors()) wattron(statwin, COLOR_PAIR(1));
        mvwprintw(statwin, 6, (win_w-strlen(header))/2, "%s", header);
        if (has_colors()) wattroff(statwin, COLOR_PAIR(1));
        FILE *f = fopen(files[sel], "rb");
        StatEntry stats[MAX_STATS];
        int count = 0;
        if (f) {
            count = fread(stats, sizeof(StatEntry), MAX_STATS, f);
            fclose(f);
        }
        for (int i = 0; i < count; i++) {
            char date[20];
            strftime(date, sizeof(date), "%Y-%m-%d", localtime(&stats[i].timestamp));
            char line[64];
            snprintf(line, sizeof(line), "%-18s %4d  %s", stats[i].name, stats[i].turns, date);
            mvwprintw(statwin, 7+i, (win_w-strlen(line))/2, "%s", line);
        }
        char *footer = "Bal/Jobb: nehezseg, ESC: vissza";
        if (has_colors()) wattron(statwin, COLOR_PAIR(1));
        mvwprintw(statwin, win_h-3, (win_w-strlen(footer))/2, "%s", footer);
        if (has_colors()) wattroff(statwin, COLOR_PAIR(1));
        wrefresh(statwin);
        ch = wgetch(statwin);
        if (ch == 27) { delwin(statwin); break; }
        if (ch == KEY_LEFT && sel > 0) sel--;
        if (ch == KEY_RIGHT && sel < 2) sel++;
    }
}

int DEBUG_MODE = 0;
#define DEBUG_LOG_LINES 8
char debug_log_lines[DEBUG_LOG_LINES][64];
int debug_log_count = 0;

void debug_log_action(const char *action, int x, int y) {
    if (!DEBUG_MODE) return;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char tstr[16];
    strftime(tstr, sizeof(tstr), "%H:%M:%S", tm_info);
    snprintf(debug_log_lines[debug_log_count % DEBUG_LOG_LINES], 64, "[%s] %s (x=%d, y=%d)", tstr, action, x, y);
    debug_log_count++;
}

int main() {
    {
        FILE *ftest = fopen("test_torpedo", "r");
        if (ftest) {
            fclose(ftest);
            printf("Automatikus teszt futtatasa...\n");
            FILE *fp = popen("./test_torpedo", "r");
            int ok = 0, fail = 0, total = 0;
            char line[256];
            if (fp) {
                while (fgets(line, sizeof(line), fp)) {
                    fputs(line, stdout); // kiírjuk a teszt sorait is
                    if (strstr(line, "OK:")) { ok++; total++; }
                    else if (strstr(line, "HIBA:")) { fail++; total++; }
                }
                int ret = pclose(fp);
                printf("Teszt eredmény: %d sikeres, %d hibás, összesen %d teszt.\n", ok, fail, total);
                if (fail == 0 && ok > 0)
                    printf("Minden teszt sikeres!\n");
                else if (fail > 0)
                    printf("FIGYELEM: %d teszt HIBÁS!\n", fail);
            } else {
                printf("Nem sikerült megnyitni a tesztet olvasásra.\n");
            }
            printf("Nyomj ENTER-t a jatek inditasahoz...\n");
            getchar();
        }
    }

    int width = CLASSIC_WIDTH, height = CLASSIC_HEIGHT, ships = CLASSIC_SHIPS, ai_level = AI_MEDIUM;
    int last_ai_level = AI_MEDIUM;
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(1, COLOR_CYAN, -1);      // Tenger (világoskék)
        init_pair(2, COLOR_YELLOW, -1);    // Miss
        init_pair(3, COLOR_RED, -1);       // Hit (süllyedt)
        init_pair(4, COLOR_GREEN, -1);     // Hajó
        init_pair(5, COLOR_GREEN, -1);     // Debug szöveg
        init_pair(6, COLOR_WHITE, COLOR_CYAN); // Keret
        init_pair(7, COLOR_MAGENTA, -1);   // Friss találat
        init_pair(8, COLOR_YELLOW, -1);    // Talált, de nem süllyedt hajó méret
        init_pair(9, COLOR_RED, -1);       // Süllyedt hajó méret
    }

    while (1) {
        int menu = main_menu();
        if (menu == 3) break; // Kilepes
        if (menu == 2) {
            show_stats_menu();
            continue;
        }
        int manual_placement = 0;
        if (menu == 1) {
            width = 10; height = 10; ships = 5; ai_level = AI_MEDIUM;
            settings_menu(&width, &height, &ships, &ai_level);
            manual_placement = 1;
        }
        if (menu == 0) {
            width = CLASSIC_WIDTH;
            height = CLASSIC_HEIGHT;
            ships = CLASSIC_SHIPS;
            ai_level = AI_MEDIUM;
            manual_placement = 0;
        }
        last_ai_level = ai_level;

        GameState game;
        init_game(&game, width, height, ships, ai_level);

        clear();
        if (manual_placement) {
            mvprintw(2, 2, "Hajok elhelyezese kovetkezik!");
            wait_for_key();
            for (int i = 0; i < game.player.height; i++)
                for (int j = 0; j < game.player.width; j++)
                    game.player.cells[i][j] = CELL_EMPTY;
            place_ships_menu(&game.player);
        } else {
            for (int i = 0; i < game.player.height; i++)
                for (int j = 0; j < game.player.width; j++)
                    game.player.cells[i][j] = CELL_EMPTY;
            place_ships(&game.player, ships, 0);
        }

        int over = 0;
        int player_special_cooldown = 0;
        int turns = 0;
        int cursor_x = 0, cursor_y = 0;
        time_t start_time = time(NULL);
        time_t last_time_update = start_time;
        int elapsed = 0;

        while (!over) {
            int scr_h, scr_w;
            getmaxyx(stdscr, scr_h, scr_w);

            int margin = 4;
            int keret_y = margin;
            int keret_x = margin;
            int keret_h = scr_h - 2 * margin;
            int keret_w = scr_w - 2 * margin;

            int grid_y = keret_y + 3;
            int grid_h = game.player.height > game.ai.height ? game.player.height : game.ai.height;
            int grid_w = game.player.width * 2;
            int ai_grid_w = game.ai.width * 2;
            int section_w = (keret_w - 10) / 3;
            int player_grid_x = keret_x + (section_w - grid_w) / 2 + 2;
            int ai_grid_x = keret_x + section_w * 2 + (section_w - ai_grid_w) / 2 + 2;
            int center_x = keret_x + section_w + (section_w / 2);

            clear();
            if (has_colors()) attron(COLOR_PAIR(6));
            for (int i = 0; i < keret_w; i++) {
                mvaddch(keret_y, keret_x + i, '#');
                mvaddch(keret_y + keret_h - 1, keret_x + i, '#');
            }
            for (int i = 0; i < keret_h; i++) {
                mvaddch(keret_y + i, keret_x, '#');
                mvaddch(keret_y + i, keret_x + keret_w - 1, '#');
            }
            mvaddch(keret_y, keret_x, '*');
            mvaddch(keret_y, keret_x + keret_w - 1, '*');
            mvaddch(keret_y + keret_h - 1, keret_x, '*');
            mvaddch(keret_y + keret_h - 1, keret_x + keret_w - 1, '*');
            if (has_colors()) attroff(COLOR_PAIR(6));

            if (DEBUG_MODE) {
                int dbg_y = keret_y + keret_h - 10;
                int dbg_x = keret_x + 2;
                if (dbg_y < keret_y + 1) dbg_y = keret_y + 1;
                if (has_colors()) attron(COLOR_PAIR(5));
                mvprintw(dbg_y, dbg_x, "[DEBUG MOD AKTIV]");
                int lines = debug_log_count < DEBUG_LOG_LINES ? debug_log_count : DEBUG_LOG_LINES;
                for (int i = 0; i < lines; i++) {
                    int idx = (debug_log_count - lines + i) % DEBUG_LOG_LINES;
                    mvprintw(dbg_y + 1 + i, dbg_x, "%s", debug_log_lines[idx]);
                }
                if (has_colors()) attroff(COLOR_PAIR(5));
            }

            int label_y = grid_y - 2;
            mvprintw(label_y, player_grid_x, "Sajat tabla:");
            mvprintw(label_y, ai_grid_x, "Ellenfel tabla:");

            int ship_info_y = grid_y + grid_h + 2;
            mvprintw(ship_info_y, player_grid_x, "Sajat hajok: %d", game.player.ships_remaining);
            mvprintw(ship_info_y, ai_grid_x, "Ellenfel hajok: %d", game.ai.ships_remaining);

            mvprintw(ship_info_y + 1, player_grid_x, "Meretek: ");
            for (int s = 0; s < game.player.ship_count; s++) {
                int sunk = (game.player.ships[s].hits == game.player.ships[s].size);
                int hit = (game.player.ships[s].hits > 0 && !sunk);
                if (sunk && has_colors()) attron(COLOR_PAIR(9));
                else if (hit && has_colors()) attron(COLOR_PAIR(8));
                printw("%d ", game.player.ships[s].size);
                if ((sunk || hit) && has_colors()) attroff(COLOR_PAIR(sunk ? 9 : 8));
            }
            mvprintw(ship_info_y + 1, ai_grid_x, "Meretek: ");
            for (int s = 0; s < game.ai.ship_count; s++) {
                int sunk = (game.ai.ships[s].hits == game.ai.ships[s].size);
                int hit = (game.ai.ships[s].hits > 0 && !sunk);
                if (sunk && has_colors()) attron(COLOR_PAIR(9));
                else if (hit && has_colors()) attron(COLOR_PAIR(8));
                printw("%d ", game.ai.ships[s].size);
                if ((sunk || hit) && has_colors()) attroff(COLOR_PAIR(sunk ? 9 : 8));
            }
            for (int i = 0; i < game.player.height; i++) {
                for (int j = 0; j < game.player.width; j++) {
                    char c = '.';
                    int color = 1;
                    if (game.player.cells[i][j] == CELL_SHIP) { c = 'H'; color = 4; }
                    if (game.player.cells[i][j] == CELL_HIT)  { c = 'X'; color = 3; }
                    if (game.player.cells[i][j] == CELL_MISS) { c = 'o'; color = 2; }
                    if (has_colors()) attron(COLOR_PAIR(color));
                    mvprintw(grid_y + i, player_grid_x + j * 2, "%c", c);
                    if (has_colors()) attroff(COLOR_PAIR(color));
                }
            }
            for (int i = 0; i < game.ai.height; i++) {
                for (int j = 0; j < game.ai.width; j++) {
                    char ec = '.';
                    int color = 1;
                    if (game.ai.cells[i][j] == CELL_HIT) {
                        ec = 'X';
                        if (game.player_turn && i == cursor_y && j == cursor_x)
                            color = 7;
                        else
                            color = 3;
                    }
                    if (game.ai.cells[i][j] == CELL_MISS) { ec = 'o'; color = 2; }
                    if (game.player_turn && i == cursor_y && j == cursor_x) {
                        attron(A_REVERSE);
                        if (has_colors()) attron(COLOR_PAIR(color));
                        mvprintw(grid_y + i, ai_grid_x + j * 2, "%c", ec);
                        if (has_colors()) attroff(COLOR_PAIR(color));
                        attroff(A_REVERSE);
                    } else {
                        if (has_colors()) attron(COLOR_PAIR(color));
                        mvprintw(grid_y + i, ai_grid_x + j * 2, "%c", ec);
                        if (has_colors()) attroff(COLOR_PAIR(color));
                    }
                }
            }
            int info_x = center_x - 12;
            int info_y = grid_y + 1;
            time_t now = time(NULL);
            if (now != last_time_update) {
                elapsed = (int)difftime(now, start_time);
                last_time_update = now;
            }
            mvprintw(info_y, info_x, "Eltelt ido: %02d:%02d", elapsed / 60, elapsed % 60);
            mvprintw(info_y + 1, info_x, "Korok szama: %d", turns + 1);
            if (player_special_cooldown > 0)
                mvprintw(info_y + 2, info_x, "Spec. visszatoltodes: %d", player_special_cooldown);
            else
                mvprintw(info_y + 2, info_x, "Spec. visszatoltodes: -");
            char *help = "Nyilak: mozgas | SPACE/ENTER: loves | a: spec. tamadas | d: spec. vedelem | q: kilep";
            int help_x = keret_x + (keret_w - (int)strlen(help)) / 2;
            mvprintw(keret_y + keret_h - 3, help_x, "%s", help);

            refresh();
            timeout(1000);
            int input = ERR;
            if (game.player_turn) input = getch();
            else input = ERR;

            if (input == KEY_F(12) || input == 0x7A) {
                DEBUG_MODE = !DEBUG_MODE;
                if (DEBUG_MODE) {
                    debug_log_count = 0;
                    memset(debug_log_lines, 0, sizeof(debug_log_lines));
                }
                continue;
            }

            if (game.player_turn) {
                if (input == 'q') { debug_log_action("kilepes", cursor_x, cursor_y); over = -2; break; }
                if ((input == 'a' || input == 'd')) {
                    int sunk = 0;
                    for (int s = 0; s < game.ai.ship_count; s++)
                        if (game.ai.ships[s].hits == game.ai.ships[s].size)
                            sunk++;
                    if (player_special_cooldown > 0 || sunk == 0) {
                        mvprintw(keret_y + keret_h - 5, help_x, "Spec. akcio csak 3 koronkent es legalabb 1 kilott hajo utan!");
                        refresh();
                        napms(1200);
                        continue;
                    }
                    player_special_cooldown = 3;
                    debug_log_action(input == 'a' ? "spec_tamadas" : "spec_vedekezes", cursor_x, cursor_y);
                    handle_player_input(&game, input, cursor_x, cursor_y);
                    game.player_turn = 0;
                    continue;
                }
                int w = game.ai.width, h = game.ai.height;
                int move = 0;
                switch (input) {
                    case KEY_UP:    if (cursor_y > 0) cursor_y--; move = 1; debug_log_action("kurzor_fel", cursor_x, cursor_y); break;
                    case KEY_DOWN:  if (cursor_y < h - 1) cursor_y++; move = 1; debug_log_action("kurzor_le", cursor_x, cursor_y); break;
                    case KEY_LEFT:  if (cursor_x > 0) cursor_x--; move = 1; debug_log_action("kurzor_bal", cursor_x, cursor_y); break;
                    case KEY_RIGHT: if (cursor_x < w - 1) cursor_x++; move = 1; debug_log_action("kurzor_jobb", cursor_x, cursor_y); break;
                }
                if (move || input == ERR) continue;
                if (input == '\n' || input == ' ') {
                    if (game.ai.cells[cursor_y][cursor_x] == CELL_EMPTY || game.ai.cells[cursor_y][cursor_x] == CELL_SHIP) {
                        debug_log_action("loves", cursor_x, cursor_y);
                        handle_player_input(&game, input, cursor_x, cursor_y);
                        game.player_turn = 0;
                    } else {
                        mvprintw(keret_y + keret_h - 5, help_x, "Erre a mezore mar lottel! Valassz masikat.");
                        refresh();
                        napms(1000);
                    }
                }
            } else if (input == ERR) {
                ai_move(&game);
                if (player_special_cooldown > 0) player_special_cooldown--;
                turns++;
                over = check_game_over(&game);
            }
            if (check_game_over(&game) != 0) over = check_game_over(&game);
        }
        clear();
        int scr_h, scr_w;
        getmaxyx(stdscr, scr_h, scr_w);
        if (over == 1) {
            char *gyoztes[] = {
                " __          _______ _   _ _   _ ______ _____    _ ",
                " \\ \\        / /_   _| \\ | | \\ | |  ____|  __ \\  | |",
                "  \\ \\  /\\  / /  | | |  \\| |  \\| | |__  | |__) | | |",
                "   \\ \\/  \\/ /   | | | . ` | . ` |  __| |  _  /  | |",
                "    \\  /\\  /   _| |_| |\\  | |\\  | |____| | \\ \\  |_|",
                "     \\/  \\/   |_____|_| \\_|_| \\_|______|_|  \\_\\ (_)",
                "                                                  "
            };
            int lines = sizeof(gyoztes)/sizeof(gyoztes[0]);
            for (int i = 0; i < lines; i++)
                mvprintw(scr_h/2 - lines + i, (scr_w-strlen(gyoztes[i]))/2, "%s", gyoztes[i]);
            if (last_ai_level == AI_ADAPTIVE) {
                char *msg = "Adaptiv nehezseg: eredmeny nem kerul be a statisztikaba.";
                mvprintw(scr_h/2+2, (scr_w-strlen(msg))/2, "%s", msg);
                char *tip;
                if (turns < 20)
                    tip = "Javasolt nehezsegi szint: NEHEZ";
                else if (turns < 35)
                    tip = "Javasolt nehezsegi szint: KOZEPES";
                else
                    tip = "Javasolt nehezsegi szint: KONNYU";
                mvprintw(scr_h/2+3, (scr_w-strlen(tip))/2, "%s", tip);
                char *cont = "Nyomj ENTER-t a folytatashoz...";
                mvprintw(scr_h/2+5, (scr_w-strlen(cont))/2, "%s", cont);
                int c;
                while ((c = getch()) != '\n' && c != '\r');
            } else {
                save_stat(last_ai_level, turns);
            }
        }
        else if (over == -2) {
            char *msg = "Jatek megszakitva.";
            mvprintw(scr_h/2, (scr_w-strlen(msg))/2, "%s", msg);
            char *cont = "Nyomj ENTER-t a folytatashoz...";
            mvprintw(scr_h/2+2, (scr_w-strlen(cont))/2, "%s", cont);
            int c;
            while ((c = getch()) != '\n' && c != '\r');
        }
        else {
            char *miwin[] = {
                "  __  __ _____  __          _______ _   _  _____   _ ",
                " |  \\/  |_   _| \\ \\        / /_   _| \\ | |/ ____| | |",
                " | \\  / | | |    \\ \\  /\\  / /  | | |  \\| | (___   | |",
                " | |\\/| | | |     \\ \\/  \\/ /   | | | . ` |\\___ \\  | |",
                " | |  | |_| |_     \\  /\\  /   _| |_| |\\  |____) | |_|",
                " |_|  |_|_____|     \\/  \\/   |_____|_| \\_|_____/  (_)"
            };
            int lines = sizeof(miwin)/sizeof(miwin[0]);
            for (int i = 0; i < lines; i++)
                mvprintw(scr_h/2 - lines + i, (scr_w-strlen(miwin[i]))/2, "%s", miwin[i]);
            char *msg = "Az MI gyozott!";
            mvprintw(scr_h/2+2, (scr_w-strlen(msg))/2, "%s", msg);
            char *cont = "Nyomj ENTER-t a folytatashoz...";
            mvprintw(scr_h/2+4, (scr_w-strlen(cont))/2, "%s", cont);
            int c;
            while ((c = getch()) != '\n' && c != '\r');
        }
        free_game(&game);
    }
    endwin();
    return 0;
}
