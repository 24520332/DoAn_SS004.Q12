#define _CRT_NONSTDC_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include "IPiece.h"
#include "OPiece.h"
#include "TPiece.h"
#include "SPiece.h"
#include "ZPiece.h"
#include "JPiece.h"
#include "LPiece.h"
#include "Bag.h"

using namespace std;

char board[H][W] = {};

// --------------
// --- Đồ họa ---
// --------------

enum Color { 
    red = 12, 
    green = 10,
    blue = 9,
    yellow = 14,
    cyan = 11,
    white = 15,
    purple = 13,
    orange = 6,
    gray = 8
};

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
}

char ColorBlock(char /* c */, int color) {
    setColor(color);
    return '\xDB';
}

char blockChar(char c) {
    switch(c) {
        case 'I': return ColorBlock(c, cyan);
        case 'O': return ColorBlock(c, yellow);
        case 'T': return ColorBlock(c, purple);
        case 'S': return ColorBlock(c, green);
        case 'Z': return ColorBlock(c, red);
        case 'J': return ColorBlock(c, blue);
        case 'L': return ColorBlock(c, orange);
        case '#': return ColorBlock(c, gray);
        case '.': return ColorBlock('.', gray);//Ghost piece
        default: return ' ';
    }
}

// ====================
// GAME STATE
// ====================

int linesCleared = 0;
int level = 1;
int x = 5, y = 0;
Piece* currentPiece = nullptr;
Piece* nextPiece = nullptr;
Bag bag;
long score = 0;
bool gameOver = false;

//Ghost Piece forward declarations
int getGhostY();
void drawGhostPiece();
void clearGhostPiece();

//Preview forward declaration
void drawNextPiece();

void gotoxy(int gotoX, int gotoY) {
    COORD c = { static_cast<SHORT>(gotoX), static_cast<SHORT>(gotoY) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

Piece* createRandomPiece() {
    int type = bag.next();
    switch(type) {
        case 0: return new IPiece();
        case 1: return new OPiece();
        case 2: return new TPiece();
        case 3: return new SPiece();
        case 4: return new ZPiece();
        case 5: return new JPiece();
        case 6: return new LPiece();
        default: return new TPiece();
    }
}

// ====================
// TIMING (GUIDELINE)
// ====================
DWORD lastFall = 0;
DWORD lockStart = 0;
const int LOCK_DELAY = 500;

// Gravity table (ms per cell)
int gravityByLevel(int lvl) {
    static int table[] = {
        1000, 793, 617, 473, 355, 262, 190,
        135, 94, 64, 43, 28, 18, 11, 7
    };
    return table[min(lvl - 1, 14)];
}

// ====================
// INPUT
// ====================
struct InputState {
    bool left = false;
    bool right = false;
    bool softDrop = false;
    bool hardDrop = false;
    bool rotate = false;
};

void readInput(InputState& in) {
    in = {};

    while (kbhit()) {
        int c = getch();
        if (c == 224) c = getch();

        if (c == 75 || c == 'a' || c == 'A') in.left = true;
        if (c == 77 || c == 'd' || c == 'D') in.right = true;
        if (c == 80 || c == 's' || c == 'S') in.softDrop = true;
        if (c == 72 || c == 'w' || c == 'W') in.rotate = true;
        if (c == ' ') in.hardDrop = true;
        if (c == 'q' || c == 'Q') gameOver = true;
    }
}

// ----------------------
// --- GAME FUNCTIONS ---
// ----------------------

void boardDelBlock() {
    if (!currentPiece) return;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece->getCell(i, j) != ' ') {
                int tx = x + j;
                int ty = y + i;
                if (ty >= 0 && ty < H && tx >= 0 && tx < W)
                    board[ty][tx] = ' ';
            }
        }
    }
}

void block2Board() {
    if (!currentPiece) return;
    
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (currentPiece->getCell(i, j) != ' ') {
                int tx = x + j;
                int ty = y + i;
                if (ty >= 0 && ty < H && tx >= 0 && tx < W)
                    board[ty][tx] = currentPiece->getCell(i, j);
            }
        }
    }
}

void initBoard() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if ((i == H - 1) || (j == 0) || (j == W - 1)) 
                board[i][j] = '#';
            else 
                board[i][j] = ' ';
}

void draw() {
    gotoxy(0, 0);
    for (int i = 0; i < H; i++, cout << '\n')
        for (int j = 0; j < W; j++) {
            cout << blockChar(board[i][j]) << blockChar(board[i][j]);
            setColor(white);
        }
    cout << "Level: " << level
     << " | Lines: " << linesCleared 
     <<" | Score: " << score << "\n";
    cout << "Press P to pause the game" << "\n";
    drawNextPiece();//Vẽ khối sẽ xuất hiện tiếp theo
}

bool canMove(int dx, int dy) {
    if (!currentPiece) return false;
    
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentPiece->getCell(i, j) != ' ') {
                int tx = x + j + dx;
                int ty = y + i + dy;

                if (tx < 1 || tx >= W - 1 || ty >= H - 1)
                    return false;
                if (ty >= 0 && board[ty][tx] != ' ')
                    return false;
            }
    return true;
}

void removeLine() {
    int cleared = 0;
    int dest = H - 2;
    
    for (int src = H - 2; src >= 0; src--) {
        bool full = true;
        for (int j = 1; j < W - 1; j++) {
            if (board[src][j] == ' ') {
                full = false;
                break;
            }
        }

        if (!full) {
            for (int j = 1; j < W - 1; j++) {
                board[dest][j] = board[src][j];
            }
            dest--;
        } else {
            cleared++;
        }
    }

    for (int i = 0; i <= dest; i++) {
        for (int j = 1; j < W - 1; j++) {
            board[i][j] = ' ';
        }
    }

    linesCleared += cleared;
    switch (cleared)
    {
    case 1:
        score += 100 * level;
        break;
    case 2:
        score += 300 * level;
        break;
    case 3:
        score += 500 * level;
        break;
    case 4:
        score += 800 * level;
        break;    
    default:
        break;
    }
}

// ====================
// LOCK PIECE
// ====================
void lockPiece() {
    block2Board();
    removeLine();
    level = linesCleared / 10 + 1;

    delete currentPiece;
    currentPiece = nextPiece;
    nextPiece = createRandomPiece();
    x = 5; y = 0;
    lockStart = 0;
    lastFall = GetTickCount();

    if (!canMove(0, 0)) gameOver = true;
}

// ----------------------
// --- GHOST PIECE ---
// ----------------------
//Tính vị trí của ghost piece
int getGhostY() {
    int ghostY = y;
    while (true) {
        bool canFall = true;

        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++) {
                if (currentPiece->getCell(i, j) != ' ') {
                    int tx = x + j;
                    int ty = ghostY + i + 1;
                    if (ty >= H - 1 || board[ty][tx] != ' ') {
                        canFall = false;
                        break;
                    }
                }
            }
        if (!canFall) break;
        ghostY++;
    }
    return ghostY;
}

//Vẽ ghost piece
void drawGhostPiece() {
    int ghostY = getGhostY();

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentPiece->getCell(i, j) != ' ') {
                int tx = x + j;
                int ty = ghostY + i;

                if (board[ty][tx] == ' ')
                    board[ty][tx] = '.';
            }
}

//Xóa sau khi vẽ
void clearGhostPiece() {
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            if (board[i][j] == '.')
                board[i][j] = ' ';
}

// ----------------------
// --- PREVIEW PIECE ---
// ----------------------
//Vẽ ô Next Piece
void drawNextPiece() {
    if (!nextPiece) return;
    
    int startX = W * 2 + 4;
    int startY = 2;

    gotoxy(startX, startY);
    cout << "NEXT: ";

    for (int i = 0; i < 4; i++) {
        gotoxy(startX, startY + i + 1);
        for (int j = 0; j < 4; j++) {
            char c = nextPiece->getCell(i, j);
            if (c != ' ')
                cout << blockChar(c) << blockChar(c);
            else
                cout << "  ";
        }
    }
}

// ----------------------
// --- MENU SYSTEM ---
// ----------------------
enum GameState { MENU, PLAYING, HOWTOPLAY, EXIT };

void showMenu(GameState& state) {
    system("cls");
    setColor(cyan);
    cout << "======== TETRIS ========\n\n";
    setColor(white);
    cout << "1. Start Game\n";
    cout << "2. How to Play\n";
    cout << "3. Exit\n\n";

    char c = static_cast<char>(getch());
    if (c == '1') state = PLAYING;
    else if (c == '2') state = HOWTOPLAY;
    else if (c == '3') state = EXIT;
}

void showHowToPlay(GameState& state) {
    system("cls");
    setColor(yellow);
    cout << "====== HOW TO PLAY ======\n\n";
    setColor(white);
    cout << "Arrow Keys / WASD: Move and rotate\n";
    cout << "W / Up Arrow: Rotate\n";
    cout << "S / Down Arrow: Soft drop\n";
    cout << "Space: Hard drop\n";
    cout << "P: Pause the game\n";
    cout << "Q: Quit\n";

    cout << "\nPress X to return to Menu\n";
    while (true) {
        char c = static_cast<char>(getch());
        if (c == 'x' || c == 'X') {
            state = MENU;
            return;
        }
    }
}

bool pauseMenu() {
    system("cls");
    setColor(yellow);
    cout << "=== GAME PAUSED ===\n\n";
    setColor(white);
    cout << "P - Continue Game\n";
    cout << "X - Return to Menu\n";

    while (true) {
        char c = static_cast<char>(getch());
        if (c == 'p' || c == 'P') return true;
        if (c == 'x' || c == 'X') return false;
    }
}

bool gameOverMenu() {
    gotoxy(0, H + 4);
    setColor(red);
    cout << "\n*** GAME OVER ***\n";
    setColor(white);
    cout << "Level: " << level
         << " | Lines: " << linesCleared
         << " | Score: " << score << "\n";
    cout << "R - Play Again\n";
    cout << "X - Return to Menu\n";

    while (true) {
        char c = static_cast<char>(getch());
        if (c == 'r' || c == 'R') return true;
        if (c == 'x' || c == 'X') return false;
    }
}

void resetGame() {
    linesCleared = 0;
    level = 1;
    score = 0;
    x = 5;
    y = 0;
    gameOver = false;
    lockStart = 0;
    lastFall = 0;
    
    initBoard();

    if (currentPiece) delete currentPiece;
    if (nextPiece) delete nextPiece;

    currentPiece = createRandomPiece();
    nextPiece = createRandomPiece();
}

void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// ====================
// UPDATE
// ====================
void updateGame(InputState& in) {
    DWORD now = GetTickCount();

    boardDelBlock();

    if (in.rotate) {
        currentPiece->rotate(x, y, board);
        lockStart = 0;
    }

    if (in.left && canMove(-1, 0)) { x--; lockStart = 0; }
    if (in.right && canMove(1, 0)) { x++; lockStart = 0; }

    if (in.hardDrop) {
        score += 10;
        while (canMove(0, 1)) y++;
        lockPiece();
        return;
    }

    DWORD fallDelay = static_cast<DWORD>(gravityByLevel(level));
    if (now - lastFall >= fallDelay || in.softDrop) {
        if (canMove(0, 1)) {
            y++;
            lastFall = now;
        } else if (!lockStart) {
            lockStart = now;
        }
    }

    if (lockStart && now - lockStart >= LOCK_DELAY)
        lockPiece();

    block2Board();
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    hideCursor();

    GameState state = MENU;

    while (state != EXIT) {

        if (state == MENU) {
            showMenu(state);
        }

        else if (state == HOWTOPLAY) {
            showHowToPlay(state);
        }

        else if (state == PLAYING) {
            system("cls");
            resetGame();

            while (!gameOver) {
                InputState input;
                readInput(input);
                
                // Check for pause
                if (input.hardDrop == false && input.softDrop == false && 
                    input.left == false && input.right == false && input.rotate == false) {
                    // Additional check for 'P' key
                    if (kbhit()) {
                        int c = getch();
                        if (c == 'p' || c == 'P') {
                            if (!pauseMenu()) {
                                state = MENU;
                                break;
                            }
                            system("cls");
                        }
                    }
                }

                updateGame(input);
                drawGhostPiece();
                draw();
                clearGhostPiece();
                Sleep(16);
            }

            if (state == PLAYING && gameOver) {
                bool replay = gameOverMenu();
                if (!replay) state = MENU;
            }
        }
    }

    if (currentPiece) delete currentPiece;
    if (nextPiece) delete nextPiece;
    return 0;
}
