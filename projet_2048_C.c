#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

void INIT_2048() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    srand(time(NULL));
}

void DONE_2048() {
    endwin();
    exit(0);
}

#define Size 4

typedef struct {
    int values[Size][Size];
} Board;

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    END
} key;

int Score = 0;
Board board;

void init_board() {
    int i, j;
    for (i = 0; i < Size; i++) {
        for (j = 0; j < Size; j++) {
            board.values[i][j] = 0;
        }
    }
}

void display_board() {
    int i, j;
    clear();
    move(0, 0);
    printw("2048 - Score: %5i", Score);
    move(1, 0);
    for (i = 0; i < Size; i++) {
        printw("+-----");
    }
    printw("+");
    for (i = 0; i < Size; i++) {
        move(i * 2 + 2, 0);
        for (j = 0; j < Size; j++) {
            printw("|");
            printw("%4i ", board.values[i][j]);
        }
        printw("|");
        move(i * 2 + 3, 0);
        for (j = 0; j < Size; j++) {
            printw("+-----");
        }
        printw("+");
    }
    refresh();
}

int count_empty() {
    int i, j, count = 0;
    for (i = 0; i < Size; i++) {
        for (j = 0; j < Size; j++) {
            if (board.values[i][j] == 0) {
                count++;
            }
        }
    }
    return count;
}

void add_two(int empty) {
    int i, j, n;
    if (empty > 0) {
        n = rand() % empty;
    } else {
        // Faire quelque chose si empty est égal à zéro
    }

    for (i = 0; i < Size; i++) {
        for (j = 0; j < Size; j++) {
            if (board.values[i][j] == 0) {
                if (n == 0) {
                    board.values[i][j] = 2;
                    return;
                }
                n--;
            }
        }
    }
}

int game_over(int add) {
    int empty = count_empty();
    int can_move = 0;
    // Vérifier si des mouvements sont encore possibles
    int i, j;
    for (i = 0; i < Size; i++) {
        for (j = 0; j < Size; j++) {
            if ((j + 1 < Size && board.values[i][j] == board.values[i][j + 1]) ||
                (i + 1 < Size && board.values[i][j] == board.values[i + 1][j])) {
                can_move = 1;
                break;
            }
        }
    }
    if (empty == 0 && !can_move) {
        move(Size * 2 + 2, 0);
        printw("Game over!");
        move(Size * 2 + 4, 0);
        refresh();
        getch();
        return 1;
    } else if (add && empty > 0) {
        int n = rand() % empty;
        add_two(n);
        display_board();
        return 0;
    } else {
        return 0;
    }
}

int shift_board() {
    int i, j, k, moved = 0;
    for (i = 0; i < Size; i++) {
        j = 0;
        while (j < Size - 1) {
            if (board.values[i][j] == 0) {
                k = j + 1;
                while (k < Size && board.values[i][k] == 0) {
                    k++;
                }
                if (k < Size) {
                    board.values[i][j] = board.values[i][k];
                    moved = 1;
                }
            }
            j++;
        }
    }
    return moved;
}

int update_board() {
    int i, j, moved = 0;
    moved |= shift_board();
    for (i = 0; i < Size; i++) {
        for (j = 0; j < Size; j++) {
            if (j + 1 < Size && board.values[i][j] == board.values[i][j + 1]) {
                board.values[i][j] *= 2;
                board.values[i][j + 1] = 0;
                Score += board.values[i][j];
                moved = 1;
            }
        }
    }
    moved |= shift_board();
    return moved;
}

key get_key() {
    int ch = getch();
    switch (ch) {
        case KEY_UP:
            return UP;
        case KEY_DOWN:
            return DOWN;
        case KEY_LEFT:
            return LEFT;
        case KEY_RIGHT:
            return RIGHT;
        default:
            return END;
    }
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void mirror_board(Board *board) {
    for (int i = 0; i < Size; i++) {
        for (int j = 0; j < Size / 2; j++) {
            swap(&board->values[i][j], &board->values[i][Size - 1 - j]);
        }
    }
}

void pivot_board(Board *board) {
    for (int i = 0; i < Size; i++) {
        for (int j = i + 1; j < Size; j++) {
            swap(&board->values[i][j], &board->values[j][i]);
        }
    }
}

int play(key dir) {
    int moved = 0;
    switch (dir) {
        case UP:
            pivot_board(&board);
            moved = update_board();
            pivot_board(&board);
            break;
        case DOWN:
            pivot_board(&board);
            mirror_board(&board);
            moved = update_board();
            mirror_board(&board);
            pivot_board(&board);
            break;
        case LEFT:
            moved = update_board();
            break;
        case RIGHT:
            mirror_board(&board);
            moved = update_board();
            mirror_board(&board);
            break;
        default:
            break;
    }
    return moved;
}

int main() {
    INIT_2048();
    init_board();
    display_board();
    int add = 1;
    key dir;
    bool score_atteint = false; // Ajout de la variable booléenne pour vérifier si le score de 2048 a été atteint

    while ((dir = get_key()) != END) {
        if (play(dir)) {
            add = 1;
        } else {
            add = 0;
        }
        display_board();
        if (Score >= 2048) {
            score_atteint = true;
            break;
        }
        game_over(add);
    }

    if (score_atteint) {
        move(Size * 2 + 2, 0);
        printw("Vous avez gagnez !");
        move(Size * 2 + 4, 0);
        refresh();
        getch();
    } else {
        move(Size * 2 + 2, 0);
        printw("Game over !");
        move(Size * 2 + 4, 0);
        refresh();
        getch();
    }

    DONE_2048();
}
