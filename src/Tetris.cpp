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
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

char ColorBlock(char c, int color) {
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

// ----------------------
// --- GAME FUNCTIONS ---
// ----------------------

int linesCleared = 0;
int level = 1;
int x = 5, y = 0;
Piece* currentPiece = nullptr;
bool gameOver = false;

void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

Piece* createRandomPiece() {
    int type = rand() % 7;
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
    for (int i = 0; i < H; i++, cout << endl)
        for (int j = 0; j < W; j++) {
            cout << blockChar(board[i][j]) << blockChar(board[i][j]);
            setColor(white);
        }

    cout << "Level: " << level << " | Lines: " << linesCleared << endl;
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

int main() {
    srand(time(0));
    
    system("cls");
    initBoard();
    
    currentPiece = createRandomPiece();
    x = 5;
    y = 0;
    
    while (!gameOver) {
        boardDelBlock();
        
        if (kbhit()) { 
            int c = getch();
            if (c == 0 || c == 224) {
                c = getch(); 
                if (c == 75 && canMove(-1, 0)) x--;
                if (c == 77 && canMove(1, 0)) x++;
                if (c == 80 && canMove(0, 1)) y++;
                if (c == 72) currentPiece->rotate(x, y, board);
            } else {
                if ((c == 'a' || c == 'A') && canMove(-1, 0)) x--;
                if ((c == 'd' || c == 'D') && canMove(1, 0)) x++;
                if ((c == 's' || c == 'S') && canMove(0, 1)) y++;
                if (c == 'w' || c == 'W') currentPiece->rotate(x, y, board);
                if (c == 'q' || c == 'Q') {
                    gameOver = true;
                }
            }
            while (kbhit()) getch();
        }

        if (canMove(0, 1)) {
            y++;
        } else {
            block2Board();
            removeLine();
            level = linesCleared / 10 + 1;
            
            delete currentPiece;
            currentPiece = createRandomPiece();
            x = 5;  // SỬA: Reset về vị trí giữa
            y = 0;

            if (!canMove(0, 0)) {
                gameOver = true;
            }
        }
        
        block2Board();
        draw();
        
        int speed = 50;
        Sleep(speed);
    }

    gotoxy(0, H + 3);
    setColor(red);
    cout << "\n*** GAME OVER ***\n";
    setColor(white);
    cout << "Final - Level: " << level << " | Lines: " << linesCleared << endl;
    cout << "Press any key to exit...";
    getch();

    delete currentPiece;
    return 0;
}
