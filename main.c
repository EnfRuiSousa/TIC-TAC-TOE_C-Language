#define _CRT_SECURE_NO_WARNINGS


#ifndef _WIN32
#define _POSIX_C_SOURCE 200809L
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <unistd.h>
#include <termios.h>
#endif

// ==========================================================
// JOGO DO GALO — C
// Campeonato melhor de 1, 3 ou 5
// Jogador: X | Computador: O
// ==========================================================

// CORES
enum {
    COL_RESET,
    COL_DIM,
    COL_CYAN,
    COL_YELLOW,
    COL_GREEN,
    COL_RED
};

static int use_ansi = 0;

#ifdef _WIN32
static HANDLE console = NULL;
static WORD original_attributes = 7;
static int console_available = 0;
#endif

// Estado do campeonato.
typedef struct {
    int best_of;
    int needed;
    int player_wins;
    int computer_wins;
    int draws;
} Match;

// ==========================================================
// 1. CORES DO TERMINAL
// ==========================================================

static void set_color(int color) {

    const int ansi[] = {
        0, 90, 96, 93, 92, 91
    };

    if (use_ansi) {
        printf("\x1b[%dm", ansi[color]);
        return;
    }

#ifdef _WIN32
    if (console_available) {

        WORD attr = original_attributes;

        switch (color) {
        case COL_DIM:
            attr = FOREGROUND_INTENSITY;
            break;

        case COL_CYAN:
            attr = FOREGROUND_GREEN |
                FOREGROUND_BLUE |
                FOREGROUND_INTENSITY;
            break;

        case COL_YELLOW:
            attr = FOREGROUND_RED |
                FOREGROUND_GREEN |
                FOREGROUND_INTENSITY;
            break;

        case COL_GREEN:
            attr = FOREGROUND_GREEN |
                FOREGROUND_INTENSITY;
            break;

        case COL_RED:
            attr = FOREGROUND_RED |
                FOREGROUND_INTENSITY;
            break;

        default:
            break;
        }

        SetConsoleTextAttribute(console, attr);
    }
#else
    (void)color;
#endif
}

// Configuracao da consola.
static void init_console(void) {

#ifdef _WIN32
    DWORD mode;
    CONSOLE_SCREEN_BUFFER_INFO info;

    console = GetStdHandle(STD_OUTPUT_HANDLE);

    if (
        console != INVALID_HANDLE_VALUE &&
        GetConsoleMode(console, &mode)
        ) {
        console_available = 1;

        if (GetConsoleScreenBufferInfo(console, &info)) {
            original_attributes = info.wAttributes;
        }

        use_ansi = SetConsoleMode(
            console,
            mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING
        ) != 0;
    }

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTitleA("Jogo do Galo - C");

#else
    use_ansi = isatty(STDOUT_FILENO);
#endif
}

// Limpa o ecra.
static void clear_screen(void) {

    if (use_ansi) {
        printf("\x1b[2J\x1b[H");

    }
    else {

#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO info;
        DWORD count, done;

        COORD home = { 0, 0 };

        if (
            console_available &&
            GetConsoleScreenBufferInfo(console, &info)
            ) {
            count =
                (DWORD)info.dwSize.X *
                (DWORD)info.dwSize.Y;

            FillConsoleOutputCharacterA(
                console, ' ', count, home, &done
            );

            FillConsoleOutputAttribute(
                console,
                original_attributes,
                count, home, &done
            );

            SetConsoleCursorPosition(console, home);

        }
        else {
            putchar('\n');
        }

#else
        if (isatty(STDOUT_FILENO)) {
            printf("\x1b[2J\x1b[H");
        }
#endif
    }
}

// ==========================================================
// 2. LER TECLAS SEM ENTER
// ==========================================================

static int read_key(void) {

#ifdef _WIN32
    int key = _getch();

    // Teclas especiais do teclado numerico.
    // Permite usar algumas delas mesmo sem Num Lock.
    if (key == 0 || key == 224) {

        int scan = _getch();

        switch (scan) {
        case 71: return '7';
        case 72: return '8';
        case 73: return '9';

        case 75: return '4';
        case 76: return '5';
        case 77: return '6';

        case 79: return '1';
        case 80: return '2';
        case 81: return '3';

        default: return 0;
        }
    }

    return key;

#else
    int key;
    struct termios old_state, raw;

    if (!isatty(STDIN_FILENO)) {
        return getchar();
    }

    if (tcgetattr(STDIN_FILENO, &old_state) != 0) {
        return getchar();
    }

    raw = old_state;

    raw.c_lflag &= (tcflag_t)~(ICANON | ECHO);

    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    key = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &old_state);

    return key;
#endif
}

// Pequena pausa para a jogada do computador.
static void delay_ms(unsigned ms) {

#ifdef _WIN32
    Sleep(ms);

#else
    struct timespec req;

    req.tv_sec = (time_t)(ms / 1000);
    req.tv_nsec = (long)(ms % 1000) * 1000000L;

    nanosleep(&req, NULL);
#endif
}

// Aguarda uma tecla de um conjunto permitido.
static int choose(const char* allowed) {

    int key;

    for (;;) {
        key = read_key();

        if (key == EOF || key == 27) {
            return -1;
        }

        if (strchr(allowed, key) != NULL) {
            return key;
        }
    }
}

// ==========================================================
// 3. CABECALHO E MENU
// ==========================================================

static void header(void) {

    printf("\n");

    set_color(COL_CYAN);

    printf("  +=========================================================+\n");
    printf("  |                  J O G O   D O   G A L O                |\n");
    printf("  |                     C  /  TERMINAL                      |\n");
    printf("  +=========================================================+\n");

    set_color(COL_RESET);
}

static void main_menu(void) {

    clear_screen();
    header();

    printf("\n");
    printf("  ESCOLHE O FORMATO DO CAMPEONATO\n\n");

    set_color(COL_YELLOW);

    printf("     [1]  Melhor de 1   (1 vitoria)\n");
    printf("     [3]  Melhor de 3   (2 vitorias)\n");
    printf("     [5]  Melhor de 5   (3 vitorias)\n");

    set_color(COL_RESET);

    printf("\n     [0]  Sair\n");

    printf("\n  REGRAS\n");

    printf("  * Tu jogas com X; o computador joga com O.\n");
    printf("  * O computador comeca a primeira partida.\n");
    printf("  * A vez de comecar alterna em cada partida.\n");
    printf("  * Os empates nao contam como vitoria.\n");

    printf("\n  Seleciona com uma tecla (sem Enter): ");

    fflush(stdout);
}

// ==========================================================
// 4. NUMERACAO DAS CASAS
// ==========================================================

// No tabuleiro os indices sao:
//
// 0 1 2
// 3 4 5
// 6 7 8
//
// No teclado numerico correspondem a:
//
// 7 8 9
// 4 5 6
// 1 2 3

static int cell_number(int index) {

    return 7 - 3 * (index / 3) + (index % 3);
}

static int cell_index(int digit) {

    int row = (9 - digit) / 3;
    int column = (digit - 1) % 3;

    return row * 3 + column;
}

// ==========================================================
// 5. VERIFICAR VITORIA OU EMPATE
// ==========================================================

// Devolve:
// 'X' -> jogador venceu
// 'O' -> computador venceu
// 'D' -> empate
// ' ' -> jogo continua

static char check_result(
    const char board[9],
    int win_cells[9]
) {

    static const int lines[8][3] = {
        { 0, 1, 2 },
        { 3, 4, 5 },
        { 6, 7, 8 },

        { 0, 3, 6 },
        { 1, 4, 7 },
        { 2, 5, 8 },

        { 0, 4, 8 },
        { 2, 4, 6 }
    };

    int i, j;

    for (i = 0; i < 9; i++) {
        win_cells[i] = 0;
    }

    // Verificar as oito combinacoes vencedoras.
    for (i = 0; i < 8; i++) {

        int a = lines[i][0];
        int b = lines[i][1];
        int c = lines[i][2];

        if (
            board[a] != ' ' &&
            board[a] == board[b] &&
            board[b] == board[c]
            ) {

            win_cells[a] = 1;
            win_cells[b] = 1;
            win_cells[c] = 1;

            return board[a];
        }
    }

    // Verificar se ainda ha casas disponiveis.
    for (j = 0; j < 9; j++) {

        if (board[j] == ' ') {
            return ' ';
        }
    }

    return 'D';
}

// ==========================================================
// 6. INTELIGENCIA DO COMPUTADOR
// ==========================================================

// Testa se um simbolo consegue ganhar
// ao ocupar alguma casa vazia.

static int move_to_win(
    char board[9],
    char symbol
) {

    int i;
    int marked[9];

    for (i = 0; i < 9; i++) {

        if (board[i] != ' ') {
            continue;
        }

        // Simular a jogada.
        board[i] = symbol;

        if (check_result(board, marked) == symbol) {

            // Repor a casa antes de devolver.
            board[i] = ' ';

            return i;
        }

        board[i] = ' ';
    }

    return -1;
}

// Estrategia do computador:
// 1. Tentar ganhar.
// 2. Bloquear vitoria imediata do jogador.
// 3. Escolher o centro.
// 4. Escolher um canto livre.
// 5. Escolher outra casa livre.

static int computer_move(char board[9]) {

    static const int corners[] = {
        0, 2, 6, 8
    };

    int options[9];
    int size = 0;

    int i, index;

    // Tentar ganhar.
    index = move_to_win(board, 'O');

    if (index >= 0) {
        return index;
    }

    // Bloquear o jogador.
    index = move_to_win(board, 'X');

    if (index >= 0) {
        return index;
    }

    // Centro.
    if (board[4] == ' ') {
        return 4;
    }

    // Cantos disponiveis.
    for (i = 0; i < 4; i++) {

        if (board[corners[i]] == ' ') {
            options[size++] = corners[i];
        }
    }

    if (size > 0) {
        return options[rand() % size];
    }

    // Qualquer casa disponivel.
    for (i = 0; i < 9; i++) {

        if (board[i] == ' ') {
            options[size++] = i;
        }
    }

    return size > 0
        ? options[rand() % size]
        : -1;
}

// ==========================================================
// 7. DESENHAR O TABULEIRO
// ==========================================================

static void draw_board(
    const char board[9],
    const int marked[9]
) {

    int row, col;

    for (row = 0; row < 3; row++) {

        printf("      +-------+-------+-------+\n");
        printf("      |       |       |       |\n");

        printf("      |");

        for (col = 0; col < 3; col++) {

            int index = 3 * row + col;
            char c = board[index];

            if (c == ' ') {

                set_color(COL_DIM);

                c = (char)(
                    '0' + cell_number(index)
                    );

            }
            else if (marked[index]) {

                // Realcar a linha vencedora.
                set_color(COL_GREEN);

            }
            else {

                set_color(
                    c == 'X'
                    ? COL_CYAN
                    : COL_YELLOW
                );
            }

            printf("   %c   ", c);

            set_color(COL_RESET);

            printf("|");
        }

        printf("\n");
        printf("      |       |       |       |\n");
    }

    printf("      +-------+-------+-------+\n");
}

// ==========================================================
// 8. DESENHAR O ESTADO DA PARTIDA
// ==========================================================

static void draw_game(
    const Match* match,
    int round,
    const char board[9],
    const int marked[9],
    const char* message
) {

    clear_screen();
    header();

    printf("\n");

    printf(
        "  PARTIDA %-3d  |  MELHOR DE %d  |  PRIMEIRO: %s\n",
        round,
        match->best_of,
        (round % 2)
        ? "COMPUTADOR"
        : "JOGADOR"
    );

    printf("  ----------------------------------------------------------\n");

    set_color(COL_CYAN);
    printf("  JOGADOR (X): %d", match->player_wins);
    set_color(COL_RESET);

    printf("     ");

    set_color(COL_YELLOW);
    printf("COMPUTADOR (O): %d", match->computer_wins);
    set_color(COL_RESET);

    printf("     EMPATES: %d\n", match->draws);

    printf(
        "  VITORIAS NECESSARIAS: %d\n\n",
        match->needed
    );

    draw_board(board, marked);

    printf("\n  ");

    set_color(COL_GREEN);
    printf("%s\n", message);
    set_color(COL_RESET);

    printf("\n");
    printf("  TECLADO NUMERICO:  7 8 9 / 4 5 6 / 1 2 3\n");
    printf("  Uma tecla = uma jogada. ESC = cancelar campeonato.\n");

    fflush(stdout);
}

// ==========================================================
// 9. CAMPEONATO
// ==========================================================

// Resultado da funcao:
//  1 -> novo campeonato
//  0 -> sair do programa
// -1 -> cancelar e regressar ao menu

static int play_match(int best_of) {

    Match match;
    int round = 0;

    match.best_of = best_of;
    match.needed = best_of / 2 + 1;

    match.player_wins = 0;
    match.computer_wins = 0;
    match.draws = 0;

    // Continuar ate alguem atingir o numero
    // de vitorias necessario.

    while (
        match.player_wins < match.needed &&
        match.computer_wins < match.needed
        ) {

        char board[9];
        char result = ' ';

        char message[120] = "";

        int marked[9] = { 0 };
        int computer_turn;
        int i;

        round++;

        // Limpar o tabuleiro.
        for (i = 0; i < 9; i++) {
            board[i] = ' ';
        }

        // Partidas impares: computador comeca.
        // Partidas pares: jogador comeca.
        computer_turn = (round % 2) == 1;

        strcpy(
            message,

            computer_turn
            ? "O computador comeca esta partida."
            : "Tu comecas esta partida."
        );

        // ==================================================
        // JOGADAS DE UMA PARTIDA
        // ==================================================

        while (result == ' ') {

            draw_game(
                &match,
                round,
                board,
                marked,

                computer_turn
                ? "Vez do computador. A pensar..."
                : message
            );

            if (computer_turn) {

                int move;

                delay_ms(550);

                move = computer_move(board);

                if (move < 0) {
                    break;
                }

                board[move] = 'O';

                snprintf(
                    message,
                    sizeof(message),

                    "O computador jogou na casa %d. A tua vez!",
                    cell_number(move)
                );

            }
            else {

                int key = read_key();
                int index;

                if (key == 27 || key == EOF) {
                    return -1;
                }

                if (key < '1' || key > '9') {

                    strcpy(
                        message,
                        "Tecla invalida: usa 1 a 9 no teclado numerico."
                    );

                    continue;
                }

                index = cell_index(key - '0');

                if (board[index] != ' ') {

                    strcpy(
                        message,
                        "Casa ocupada! Escolhe outra casa."
                    );

                    continue;
                }

                board[index] = 'X';

                strcpy(message, "Boa jogada!");
            }

            // Verificar o estado do jogo.
            result = check_result(
                board,
                marked
            );

            // Alternar o jogador.
            computer_turn = !computer_turn;
        }

        // ==================================================
        // RESULTADO DA PARTIDA
        // ==================================================

        if (result == 'X') {

            match.player_wins++;

            strcpy(
                message,
                "VITORIA! Ganhaste esta partida."
            );

        }
        else if (result == 'O') {

            match.computer_wins++;

            strcpy(
                message,
                "O computador ganhou esta partida."
            );

        }
        else {

            match.draws++;

            strcpy(
                message,
                "EMPATE! Esta partida nao conta como vitoria."
            );
        }

        draw_game(
            &match,
            round,
            board,
            marked,
            message
        );

        // Se ja temos campeao, terminar.
        if (
            match.player_wins >= match.needed ||
            match.computer_wins >= match.needed
            ) {
            break;
        }

        printf(
            "\n  Prime uma tecla para a partida seguinte... "
        );

        fflush(stdout);

        if (read_key() == 27) {
            return -1;
        }
    }

    // ======================================================
    // RESULTADO FINAL DO CAMPEONATO
    // ======================================================

    clear_screen();
    header();

    printf("\n");
    printf("  +---------------------------------------------------------+\n");

    if (match.player_wins > match.computer_wins) {

        set_color(COL_GREEN);

        printf(
            "  |              CAMPEAO: JOGADOR!                         |\n"
        );

    }
    else {

        set_color(COL_YELLOW);

        printf(
            "  |              CAMPEAO: COMPUTADOR                       |\n"
        );
    }

    set_color(COL_RESET);

    printf("  +---------------------------------------------------------+\n\n");

    printf(
        "  RESULTADO FINAL (MELHOR DE %d)\n\n",
        match.best_of
    );

    printf(
        "  Jogador: %d    Computador: %d    Empates: %d\n",
        match.player_wins,
        match.computer_wins,
        match.draws
    );

    printf("\n");
    printf("  [1] Novo campeonato     [0] Sair\n");
    printf("\n  Escolhe: ");

    fflush(stdout);

    return choose("10") == '1';
}

// ==========================================================
// 10. FUNCAO PRINCIPAL
// ==========================================================

int main(void) {

    int choice;

    init_console();

    // Inicializar os numeros aleatorios.
    srand((unsigned)time(NULL));

    for (;;) {

        main_menu();

        choice = choose("0135");

        if (choice == '0' || choice == -1) {
            break;
        }

        // Comecar campeonato.
        if (play_match(choice - '0') == 0) {
            break;
        }
    }

    clear_screen();

    set_color(COL_CYAN);

    printf("\n  Obrigado por jogar. Ate a proxima!\n\n");

    set_color(COL_RESET);

    return 0;
}
