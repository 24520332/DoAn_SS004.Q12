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
Bag bag;
long score = 0;
bool gameOver = false;

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
    currentPiece = createRandomPiece();
    x = 5; y = 0;
    lockStart = 0;
    lastFall = GetTickCount();

    if (!canMove(0, 0)) gameOver = true;
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
    
    system("cls");
    initBoard();
    
    currentPiece = createRandomPiece();
    
    while (!gameOver) {
        InputState input;
        readInput(input);
        updateGame(input);
        draw();      
        Sleep(16);
    }

    gotoxy(0, H + 3);
    setColor(red);
    cout << "\n*** GAME OVER ***\n";
    setColor(white);
    cout << "Level: " << level
     << " | Lines: " << linesCleared 
     <<" | Score: " << score << "\n";
    cout << "Press any key to exit...";
    getch();

    delete currentPiece;
    return 0;
}
