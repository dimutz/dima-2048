/* Programarea Calculatoarelor, seria CC
 * Tema2 - 2048
 */
#include <ctype.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>

#define FOREVER 1
#define MOVE_TIMER 5
#define S_TO_WAIT 1
#define MILIS_TO_WAIT 0

#define GRID_CENTER_Y 3
#define GRID_CENTER_X 4
#define GAME_HEIGHT 27
#define GAME_WIDTH 85
#define GRID_WIDTH 43
#define MENU_HEIGHT 15
#define PANEL_WIDTH (GAME_WIDTH - GRID_WIDTH)

#define KEYBOARD 0
#define SELECT_EVENT 1
#define SELECT_NO_EVENT 0

/* functie pentru a genera aleatoriu cele 2 pozitii ocupate
   la inceput de joc, cat si numerele aflate pe acele pozitii */
void randomStart(int* poz1, int* poz2, int* nr1, int* nr2) {
    *poz1 = rand() % 16;
    do {
        *poz2 = rand() % 16;
    } while (*poz2 == *poz1);
    /* se foloseste un seed pentru sansa de 10% ca un numar sa fie 4 */
    int seed1 = rand() % 100;
    if (seed1 < 10) {
        *nr1 = 2;
    } else
        *nr1 = 1;
    int seed2 = rand() % 100;
    if (seed2 < 10) {
        *nr2 = 2;
    } else
        *nr2 = 1;
}

/* functie pentru a numara cate celule sunt libere pe tabla de joc */
int countFree(int* table) {
    int i, nr = 0;
    for (i = 0; i < 16; i++) {
        if (!table[i]) {
            nr++;
        }
    }
    return nr;
}

/* functie pentru adaugarea unui numar pe o pozitie aleatoare */
void addRandom(int* table) {
    int poz;
    do {
        poz = rand() % 16;
    } while (table[poz]);
    int seed = rand() % 100;
    if (seed < 10) {
        table[poz] = 2;
    } else {
        table[poz] = 1;
    }
}

/* functie pentru efectuarea unei mutari;
   1 -> up; 2 -> left; 3 -> down; 4 -> right */
int moveTable(int* table, int* merged, int moveNr) {
    int i, j, k, l, ok, moved = 0;
    int v[4];
    /* se extrage fiecare linie/coloana intr-un vector */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            /* se determina ordinea in functie de mutare */
            int aux1 = j, aux2 = 4 * j + i;
            if (moveNr > 2) {
                aux1 = 3 - j;
            }
            if (moveNr % 2 == 0) {
                aux2 = 4 * i + j;
            }
            v[aux1] = table[aux2];
        }
        ok = 1;
        while (ok) {
            ok = 0;
            /* se muta toate valorile de 0 la final */
            for (k = 0; k < 3; k++) {
                int ok1 = 1;
                for (l = k + 1; l < 4; l++) {
                    if (v[l]) {
                        ok1 = 0;
                    }
                }
                if (v[k] == 0 && !ok1) {
                    while (k < 3) {
                        v[k] = v[k + 1];
                        /* se determina pozitia numerelor interschimbate
                           in functie de mutare */
                        int aux1 = 4 * k + i, aux2 = 4 * (k + 1) + i;
                        if (moveNr % 2 == 0) {
                            aux1 = 4 * i + k;
                            aux2 = aux1 + 1;
                        }
                        merged[aux1] = merged[aux2];
                        k++;
                    }
                    k = -1;
                    v[3] = 0;
                    int aux = 12 + i;
                    if (moveNr % 2 == 0) {
                        aux = 4 * i + 3;
                    }
                    merged[aux] = 0;
                    moved = 1;
                }
            }
            /* se unesc valorile adiacente egale */
            for (k = 0; k < 3; k++) {
                /* se determina pozitia numerelor interschimbate
                   in functie de mutare */
                int aux1 = 4 * k + i, aux2 = 4 * (k + 1) + i;
                if (moveNr % 2 == 0) {
                    aux1 = 4 * i + k;
                    aux2 = aux1 + 1;
                }
                if (v[k] == v[k + 1] && v[k] && !merged[aux1] &&
                    !merged[aux2]) {
                    v[k]++;
                    v[k + 1] = 0;
                    ok = 1;
                    merged[aux1] = 1;
                    moved = 1;
                }
            }
        }
        /* se reflecta schimbarile pe tabla de joc */
        for (j = 0; j < 4; j++) {
            int aux1 = j, aux2 = 4 * j + i;
            if (moveNr > 2) {
                aux1 = 3 - j;
            }
            if (moveNr % 2 == 0) {
                aux2 = 4 * i + j;
            }
            table[aux2] = v[aux1];
        }
        /* la mutarile down/right trebuie inversat si tabloul merged */
        if (moveNr == 3) {
            int aux = merged[i];
            merged[i] = merged[12 + i];
            merged[12 + i] = aux;
            aux = merged[4 + i];
            merged[4 + i] = merged[8 + i];
            merged[8 + i] = aux;
        } else if (moveNr == 4) {
            int aux = merged[4 * i];
            merged[4 * i] = merged[4 * i + 3];
            merged[4 * i + 3] = aux;
            aux = merged[4 * i + 1];
            merged[4 * i + 1] = merged[4 * i + 2];
            merged[4 * i + 2] = aux;
        }
    }
    /* returneaza 0 daca nu au fost schimbari si 1 in caz contrar*/
    return moved;
}

/* functie pentru a verifica daca o mutare este valida */
int checkMove(int* table, int moveNr) {
    int i, *copyTable, *auxMerged, canMove, merges = 0;
    copyTable = (int*)malloc(16 * sizeof(int));
    auxMerged = (int*)calloc(16, sizeof(int));
    for (i = 0; i < 16; i++) {
        copyTable[i] = table[i];
    }
    canMove = moveTable(copyTable, auxMerged, moveNr);
    /* se numara celulele ce ar putea fi eliberate */
    if (canMove) {
        for (i = 0; i < 16; i++) {
            if (auxMerged[i]) {
                merges++;
            }
        }
    }
    free(copyTable);
    free(auxMerged);
    /* daca mutarea este valida, se returneaza si canMove in cazul
       in care merges ramane 0; daca mutarea este invalida,
       si merges va fi automat 0*/
    return canMove + merges;
}

/* functie pentru incrementarea scorului*/
void checkScore(int* table, int* merged, int* score) {
    int i;
    for (i = 0; i < 16; i++) {
        if (merged[i]) {
            *score += 1 << table[i];
        }
    }
}

/* functie pentru a verifica daca jocul a fost castigat */
int checkWin(int* table) {
    int i;
    for (i = 0; i < 16; i++) {
        if (1 << table[i] == 2048) return 1;
    }
    return 0;
}

/* functie pentru actualizarea ferestrei grid*/
void updateGrid(WINDOW** boxes, int* game) {
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_BLACK);
    init_pair(8, COLOR_WHITE, COLOR_BLUE);
    init_pair(9, COLOR_WHITE, COLOR_MAGENTA);
    init_pair(10, COLOR_WHITE, COLOR_RED);
    init_pair(11, COLOR_BLACK, COLOR_WHITE);
    int i, j;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            int nr = (game[4 * i + j] - 1) / 3;
            /* se afiseaza numarul aflat in fiecare celula */
            mvwprintw(boxes[4 * i + j], GRID_CENTER_Y, 2, "     ");
            wmove(boxes[4 * i + j], GRID_CENTER_Y,
                  GRID_CENTER_X - (nr - 1) / 2);
            if (game[4 * i + j]) {
                wattron(boxes[4 * i + j], COLOR_PAIR(game[4 * i + j]));
                wprintw(boxes[4 * i + j], "%d", 1 << game[4 * i + j]);
                wattroff(boxes[4 * i + j], COLOR_PAIR(1));
                while (nr < 3) {
                    wprintw(boxes[4 * i + j], " ");
                    nr++;
                }
            } else {
                wprintw(boxes[4 * i + j], "    ");
            }
            wrefresh(boxes[4 * i + j]);
        }
    }
}

/* functie pentru actualizarea ferestrei panel*/
void updatePanel(WINDOW* panel, int* table, int score, int gameOver) {
    wclear(panel);
    time_t t;
    time(&t);
    struct tm* local_time = localtime(&t);
    mvwprintw(panel, 2, PANEL_WIDTH / 2 - 4, "DIMA 2048");
    mvwprintw(panel, 4, PANEL_WIDTH / 2 - 11, "%s", asctime(local_time));
    mvwprintw(panel, 6, PANEL_WIDTH / 2 - 5, "SCORE: %d", score);
    /* daca gameOver = 0 jocul poate fi continuat;
       daca gameOver = 1 jocul a fost pierdut;
       daca gameOver = 2 jocul a fost castigat.*/
    if (gameOver) {
        mvwprintw(panel, 8, PANEL_WIDTH / 2 - 4, "GAME OVER");
        if (gameOver == 1) {
            init_pair(12, COLOR_WHITE, COLOR_GREEN);
            wattron(panel, COLOR_PAIR(12));
            mvwprintw(panel, 9, PANEL_WIDTH / 2 - 4, "YOU WIN");
            wattroff(panel, COLOR_PAIR(12));
        }
        if (gameOver == 2) {
            init_pair(12, COLOR_WHITE, COLOR_RED);
            wattron(panel, COLOR_PAIR(12));
            mvwprintw(panel, 9, PANEL_WIDTH / 2 - 4, "YOU LOSE");
            wattroff(panel, COLOR_PAIR(12));
        }
    }
    /* legenda */
    int i = 11;
    if (checkMove(table, 1)) {
        mvwprintw(panel, i, PANEL_WIDTH / 2 - 6, "W - MOVE UP");
        i++;
    }
    if (checkMove(table, 2)) {
        mvwprintw(panel, i, PANEL_WIDTH / 2 - 6, "A - MOVE LEFT");
        i++;
    }
    if (checkMove(table, 3)) {
        mvwprintw(panel, i, PANEL_WIDTH / 2 - 6, "S - MOVE DOWN");
        i++;
    }
    if (checkMove(table, 4)) {
        mvwprintw(panel, i, PANEL_WIDTH / 2 - 6, "D - MOVE RIGHT");
        i++;
    }
    mvwprintw(panel, i, PANEL_WIDTH / 2 - 6, "U - UNDO MOVE");
    mvwprintw(panel, i + 1, PANEL_WIDTH / 2 - 6, "Q - QUIT TO MENU");
    box(panel, 0, 0);
    /* se reflecta schimbarile pe ecran */
    wrefresh(panel);
}

/* functie pentru fereastra menu */
void inMenu(int* isIngame, int* isInprogress, int* quit) {
    int row, col, new_row, new_col;
    int nrows, ncols, c;
    getmaxyx(stdscr, nrows, ncols);
    /* initializez fereastra */
    WINDOW* menu =
        newwin(MENU_HEIGHT, ncols / 2, (nrows - MENU_HEIGHT) / 2, ncols / 4);
    box(menu, 0, 0);
    int aux = getmaxx(menu) / 2;
    mvwaddstr(menu, 3, aux - 4, "DIMA 2048");
    mvwaddstr(menu, 5, aux - 10, "PLEASE SELECT OPTION:");
    mvwaddstr(menu, 7, aux - 3, "NEW GAME");
    if (!*isInprogress) {
        wattron(menu, A_DIM);
    }
    mvwaddstr(menu, 9, aux - 3, "RESUME");
    if (!*isInprogress) {
        wattroff(menu, A_DIM);
    }
    mvwaddstr(menu, 11, aux - 3, "QUIT");
    wmove(menu, 7, aux - 8);
    getyx(menu, row, col);
    wattron(menu, A_BLINK);
    waddstr(menu, "-->");
    /* se reflecta schimbarile pe ecran*/
    wrefresh(menu);
    int option = 1;
    /* permit input de la keypad */
    keypad(menu, TRUE);
    /* se alege optiunea dorita */
    while (!*isIngame && !*quit) {
        c = wgetch(menu);
        switch (c) {
            case KEY_UP: {
                if (row != 7) {
                    new_row = row - 2;
                    new_col = col;
                    option--;
                } else {
                    new_row = row + 4;
                    new_col = col;
                    option += 2;
                }
                break;
            }
            case KEY_DOWN: {
                if (row != 11) {
                    new_row = row + 2;
                    new_col = col;
                    option++;
                } else {
                    new_row = row - 4;
                    new_col = col;
                    option -= 2;
                }
                break;
            }
            case 10: {
                switch (option) {
                    case 1: {
                        *isIngame = 1;
                        *isInprogress = 0;
                        break;
                    }
                    case 2: {
                        if (*isInprogress) {
                            *isIngame = 1;
                        }
                        break;
                    }
                    case 3: {
                        *quit = 1;
                        break;
                    }
                }
            }
            default: {
                new_col = col;
                new_row = row;
            }
        }
        mvwaddstr(menu, row, col, "   ");
        wmove(menu, new_row, new_col);
        waddstr(menu, "-->");
        wrefresh(menu);
        row = new_row;
        col = new_col;
    }
    wattroff(menu, A_BLINK);
    /* se sterg ecranul si fereastra */
    clear();
    delwin(menu);
    refresh();
}

/* functie pentru fereastra game */
void inGame(int* isIngame, int* isInprogress, int* table, int* score,
            int* lastTurn, int* lastScore) {
    int nrows, ncols, nfds = 1, sel, timer = 0;
    int i, j, poz1, poz2, nr1, nr2, gameOver = 0;
    /* merged reprezinta tabla de joc la fiecare pas, astfel:
       1 daca pe celula respectiva a avut loc o adunare;
       0 in caz contrar
       cu ajutorul lui se incrementeaza scorul */
    int* merged;
    fd_set read_descriptors;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    getmaxyx(stdscr, nrows, ncols);
    /* se initalizeaza fereastra game care se imparte in 2 mai mici:
       grid - tabla de joc si panel - panoul de control */
    WINDOW* game = newwin(GAME_HEIGHT, GAME_WIDTH, (nrows - GAME_HEIGHT) / 2,
                          (ncols - GAME_WIDTH) / 2);
    WINDOW* grid = derwin(game, GAME_HEIGHT, GRID_WIDTH, 0, 0);
    WINDOW* panel = derwin(game, 27, PANEL_WIDTH, 0, GRID_WIDTH);
    WINDOW* boxes[16];
    box(game, 0, 0);
    box(grid, 0, 0);
    wtimeout(game, 0);
    wrefresh(game);
    /* in fereastra grid, fiecare celula primeste fereastra ei proprie */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            boxes[4 * i + j] = derwin(grid, 7, 11, 1 + 6 * i, 1 + 10 * j);
            wborder(boxes[4 * i + j], 0, 0, ACS_HLINE, ACS_HLINE, ACS_PLUS,
                    ACS_PLUS, ACS_PLUS, ACS_PLUS);
            wrefresh(boxes[4 * i + j]);
        }
    }
    /* isInprogress = 0 inseamna ca s-a intrat in joc folosind NEW GAME
       si se pregateste tabla de joc */
    if (!*isInprogress) {
        for (i = 0; i < 16; i++) {
            table[i] = 0;
            lastTurn[i] = 0;
        }
        *score = 0;
        *lastScore = 0;
        randomStart(&poz1, &poz2, &nr1, &nr2);
        table[poz1] = nr1;
        table[poz2] = nr2;
        lastTurn[poz1] = nr1;
        lastTurn[poz2] = nr2;
    }
    /* se reflecta schimbarile pe ecran */
    updateGrid(boxes, table);
    updatePanel(panel, table, *score, gameOver);
    while (*isIngame) {
        updatePanel(panel, table, *score, gameOver);
        FD_ZERO(&read_descriptors);
        FD_SET(KEYBOARD, &read_descriptors);
        /* select cu timeout 1 secunda pentru actualizarea constanta a orei */
        sel = select(nfds, &read_descriptors, NULL, NULL, &timeout);
        /* variabila timer reprezinta limita pana la mutarea automata
           si se incrementeaza dupa fiecare timeout de o secunda */
        if (timer != MOVE_TIMER) {
            /* se citeste operatia dorita */
            int moved, c = wgetch(game);
            merged = (int*)calloc(16, sizeof(int));
            switch (tolower(c)) {
                /* operatia QUIT */
                case 'q': {
                    /* se sterg ecranul si fereastra; isIngame = 0
                    inseamna ca trebuie sa se revina in meniu */
                    clear();
                    delwin(game);
                    *isIngame = 0;
                    break;
                }
                /* operatia MOVE UP */
                case 'w': {
                    /* se efectueaza operatia daca jocul inca merge
                       si daca mutarea este valida */
                    if (!gameOver && checkMove(table, 1)) {
                        /* se retin tabla si scorul
                           pentru un posibil viitor UNDO */
                        for (i = 0; i < 16; i++) {
                            lastTurn[i] = table[i];
                        }
                        *lastScore = *score;
                        moved = moveTable(table, merged, 1);
                        /* daca mutarea este valida si exista
                           spatii libere, se adauga un numar */
                        if (moved && countFree(table)) {
                            addRandom(table);
                        };
                        /* se reflecta schimbarile pe ecran */
                        updateGrid(boxes, table);
                        checkScore(table, merged, score);
                    }
                    break;
                }
                /* operatia MOVE LEFT */
                case 'a': {
                    /* se efectueaza operatia daca jocul inca merge
                       si daca mutarea este valida */
                    if (!gameOver && checkMove(table, 2)) {
                        /* se retin tabla si scorul
                           pentru un posibil viitor UNDO */
                        for (i = 0; i < 16; i++) {
                            lastTurn[i] = table[i];
                        }
                        *lastScore = *score;
                        moved = moveTable(table, merged, 2);
                        /* daca mutarea este valida si exista
                           spatii libere, se adauga un numar */
                        if (moved && countFree(table)) {
                            addRandom(table);
                        };
                        /* se reflecta schimbarile pe ecran */
                        updateGrid(boxes, table);
                        checkScore(table, merged, score);
                    }
                    break;
                }
                /* operatia MOVE DOWN */
                case 's': {
                    /* se efectueaza operatia daca jocul inca merge
                       si daca mutarea este valida */
                    if (!gameOver && checkMove(table, 3)) {
                        /* se retin tabla si scorul
                           pentru un posibil viitor UNDO */
                        for (i = 0; i < 16; i++) {
                            lastTurn[i] = table[i];
                        }
                        *lastScore = *score;
                        moved = moveTable(table, merged, 3);
                        /* daca mutarea este valida si exista
                           spatii libere, se adauga un numar */
                        if (moved && countFree(table)) {
                            addRandom(table);
                        };
                        /* se reflecta schimbarile pe ecran */
                        updateGrid(boxes, table);
                        checkScore(table, merged, score);
                    }
                    break;
                }
                /* operatia MOVE RIGHT */
                case 'd': {
                    /* se efectueaza operatia daca jocul inca merge
                       si daca mutarea este valida */
                    if (!gameOver && checkMove(table, 4)) {
                        /* se retin tabla si scorul
                           pentru un posibil viitor UNDO */
                        for (i = 0; i < 16; i++) {
                            lastTurn[i] = table[i];
                        }
                        *lastScore = *score;
                        moved = moveTable(table, merged, 4);
                        /* daca mutarea este valida si exista
                           spatii libere, se adauga un numar */
                        if (moved && countFree(table)) {
                            addRandom(table);
                        };
                        /* se reflecta schimbarile pe ecran */
                        updateGrid(boxes, table);
                        checkScore(table, merged, score);
                    }
                    break;
                }
                /* operatia UNDO */
                case 'u': {
                    /* daca gameOver = 1 inseamna ca jocul a fost
                       castigat si operatia nu are rost */
                    if (gameOver != 1) {
                        /* se revine la starea anterioara */
                        for (i = 0; i < 16; i++) {
                            table[i] = lastTurn[i];
                        }
                        *score = *lastScore;
                        /* se reflecta schimbarile pe ecran */
                        updateGrid(boxes, table);
                        break;
                    }
                    /* operatia UNDO poate fi efectuata o singura data:
                       se poate reveni numai la cea mai recenta stare */
                }
            }
            free(merged);
        } else {
            /* timerul a expirat si se efectueaza mutarea automata */
            int max, todo, moved, available[4] = {0}, nr = 0;
            int upMerges, downMerges, leftMerges, rightMerges;
            /* se verifica pentru fiecare mutare daca este valida
               si cate celule sunt eliberate */
            merged = (int*)calloc(16, sizeof(int));
            upMerges = checkMove(table, 1);
            leftMerges = checkMove(table, 2);
            downMerges = checkMove(table, 3);
            rightMerges = checkMove(table, 4);
            max = upMerges;
            available[nr++] = 1;
            if (leftMerges > max) {
                max = leftMerges;
                nr = 0;
                available[nr++] = 2;
            } else if (leftMerges == max) {
                available[nr++] = 2;
            }
            if (downMerges > max) {
                max = downMerges;
                nr = 0;
                available[nr++] = 3;
            } else if (downMerges == max) {
                available[nr++] = 3;
            }
            if (rightMerges > max) {
                max = rightMerges;
                nr = 0;
                available[nr++] = 4;
            } else if (rightMerges == max) {
                available[nr++] = 4;
            }
            /* se alege la intamplare una din mutarile valide determinate */
            todo = available[rand() % nr];
            /* se efectueaza mutarea determinata anterior
               si se reflecta schimbarile pe ecran */
            if (!gameOver && checkMove(table, todo)) {
                for (i = 0; i < 16; i++) {
                    lastTurn[i] = table[i];
                }
                *lastScore = *score;
                moved = moveTable(table, merged, todo);
                if (moved && countFree(table)) {
                    addRandom(table);
                };
                updateGrid(boxes, table);
                checkScore(table, merged, score);
            }
            free(merged);
            /* timerul este resetat */
            timer = 0;
        }
        /* se verifica starea jocului */
        gameOver = checkWin(table);
        if (gameOver == 0) {
            if (checkMove(table, 1) + checkMove(table, 2) +
                checkMove(table, 3) + checkMove(table, 4)) {
                gameOver = 0;
            } else {
                gameOver = 2;
            }
        }
        switch (sel) {
            /* la primirea de input se reseteaza timerul */
            case SELECT_EVENT: {
                timer = 0;
                break;
            }
            /* altfel timerul merge in continuare */
            case SELECT_NO_EVENT: {
                timer++;
                break;
            }
        }
        /* se actualizeaza panoul de control */
        updatePanel(panel, table, *score, gameOver);
        FD_SET(KEYBOARD, &read_descriptors);
        /* se reseteaza timeout */
        timeout.tv_sec = S_TO_WAIT;
        timeout.tv_usec = MILIS_TO_WAIT;
    }
    /* conditii pentru revenirea in meniu */
    *isInprogress = !gameOver;
    *isIngame = 0;
    clear();
    for (i = 0; i < 16; i++) {
        delwin(boxes[i]);
    }
    delwin(grid);
    delwin(panel);
    delwin(game);
    refresh();
}

int main(void) {
    int isIngame = 0, isInprogress = 0, quit = 0;
    int *game, score, *lastTurn, lastScore;
    srand(time(NULL));
    /* se initializeaza ecranul */
    initscr();
    /* se sterge ecranul */
    clear();
    /* se inhiba afisarea caracterelor introduse la tastatura */
    noecho();
    /* caracterele introduse sunt citite imediat */
    cbreak();
    /* se ascunde cursorul */
    curs_set(0);
    /* se permite folosirea culorilor */
    start_color();
    while (FOREVER) {
        /* isIngame = 0 -> meniu; isIngame = 1 - > joc
           isInprogress = 0 -> nu se poate accesa resume
           isInprogress = 1 -> se poate accesa si resume
           quit = 1 -> se iese din aplicatie */
        if (!isIngame) {
            if (!isInprogress) {
                game = (int*)calloc(16, sizeof(int));
                lastTurn = (int*)calloc(16, sizeof(int));
            }
            inMenu(&isIngame, &isInprogress, &quit);
        }
        if (isIngame) {
            inGame(&isIngame, &isInprogress, game, &score, lastTurn,
                   &lastScore);
        }
        if (quit) {
            break;
        }
    }
    free(game);
    free(lastTurn);
    endwin();
    return 0;
}