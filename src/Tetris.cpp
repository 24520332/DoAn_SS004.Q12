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

string ColorBlock(char /* c */, int color) {
    setColor(color);
    return "\u2588"; // Unicode full block █
}

string blockChar(char c) {
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
        default: return " ";
    }
}

// ====================
// GAME STATE
// ====================

int linesCleared = 0;
int level = 1;
int startingLevel = 1; // Level mà người chơi chọn khi bắt đầu
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

// Gravity table (ms per cell) - 5 cấp độ
int gravityByLevel(int lvl) {
    static int table[] = {
        1000,  // Level 1: Rất chậm (1 giây)
        700,   // Level 2: Chậm 
        450,   // Level 3: Trung bình
        250,   // Level 4: Nhanh
        100    // Level 5: Rất nhanh
    };
    if (lvl < 1) lvl = 1;
    if (lvl > 5) lvl = 5;
    return table[lvl - 1];
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
    bool pause = false;
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
        if (c == 'p' || c == 'P') {
            in.pause = true;
        }
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
    // Vẽ board
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            cout << blockChar(board[i][j]) << blockChar(board[i][j]);
            setColor(white);
        }
        cout << '\n';
    }
    
    // Vẽ thông tin game
    cout << "Level: " << level
         << " | Lines: " << linesCleared 
         << " | Score: " << score << "       \n"; // Thêm spaces để xóa text cũ
    cout << "Press P to pause the game       \n";
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
    // Level giữ nguyên, không tăng tự động

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
enum GameState { MENU, PLAYING, HOWTOPLAY, SELECTLEVEL, EXIT };

void selectLevel(GameState& state) {
    system("cls");
    setColor(cyan);
    cout << "\n\n";
    cout << "         ╔═══════════════════════════════════════════════════════╗\n";
    cout << "         ║                    CHON CAP DO                        ║\n";
    cout << "         ╚═══════════════════════════════════════════════════════╝\n";
    cout << "\n";
    setColor(white);
    cout << "                  ╔═══════════════════════════════════╗\n";
    cout << "                  ║  Cap do hien tai: ";
    setColor(yellow);
    cout << "Level " << startingLevel << "      ";
    setColor(white);
    cout << "║\n";
    cout << "                  ╠═══════════════════════════════════╣\n";
    cout << "                  ║  Toc do roi: ";
    if (startingLevel == 1) {
        setColor(green);
        cout << "RAT CHAM       ";
    } else if (startingLevel == 2) {
        setColor(green);
        cout << "CHAM           ";
    } else if (startingLevel == 3) {
        setColor(yellow);
        cout << "TRUNG BINH     ";
    } else if (startingLevel == 4) {
        setColor(orange);
        cout << "NHANH          ";
    } else {
        setColor(red);
        cout << "RAT NHANH      ";
    }
    setColor(white);
    cout << "║\n";
    cout << "                  ╚═══════════════════════════════════╝\n";
    cout << "\n";
    setColor(gray);
    cout << "                 ⚠ Toc do se giu nguyen trong suot game\n\n";
    setColor(white);
    cout << "                  ╔═══════════════════════════════════╗\n";
    setColor(green);
    cout << "                  ║  [+] ▲ Tang cap do               ║\n";
    setColor(red);
    cout << "                  ║  [-] ▼ Giam cap do               ║\n";
    setColor(yellow);
    cout << "                  ║  [ENTER] ▶ Bat dau choi          ║\n";
    setColor(cyan);
    cout << "                  ║  [X] ◄ Quay lai Menu             ║\n";
    setColor(white);
    cout << "                  ╚═══════════════════════════════════╝\n";
    cout << "\n";
    setColor(gray);
    cout << "                         (Cap do: 1-5)\n";
    setColor(white);

    while (true) {
        char c = static_cast<char>(getch());
        if (c == '+' || c == '=') {
            if (startingLevel < 5) {
                startingLevel++;
                selectLevel(state);
                return;
            }
        }
        else if (c == '-' || c == '_') {
            if (startingLevel > 1) {
                startingLevel--;
                selectLevel(state);
                return;
            }
        }
        else if (c == '\r' || c == '\n') {
            state = PLAYING;
            return;
        }
        else if (c == 'x' || c == 'X') {
            state = MENU;
            return;
        }
    }
}

void showMenu(GameState& state) {
    system("cls");
    setColor(cyan);
    cout << "\n\n";
    cout << "         ╔═══════════════════════════════════════════════════════╗\n";
    cout << "         ║                                                       ║\n";
    setColor(yellow);
    cout << "         ║      ████████╗███████╗████████╗██████╗ ██╗███████╗    ║\n";
    cout << "         ║      ╚══██╔══╝██╔════╝╚══██╔══╝██╔══██╗██║██╔════╝    ║\n";
    cout << "         ║         ██║   █████╗     ██║   ██████╔╝██║███████╗    ║\n";
    cout << "         ║         ██║   ██╔══╝     ██║   ██╔══██╗██║╚════██║    ║\n";
    cout << "         ║         ██║   ███████╗   ██║   ██║  ██║██║███████║    ║\n";
    cout << "         ║         ╚═╝   ╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═╝╚══════╝    ║\n";
    setColor(cyan);
    cout << "         ║                                                       ║\n";
    cout << "         ╚═══════════════════════════════════════════════════════╝\n";
    cout << "\n";
    setColor(white);
    cout << "                  ╔═══════════════════════════════════╗\n";
    cout << "                  ║         MENU CHINH               ║\n";
    cout << "                  ╠═══════════════════════════════════╣\n";
    setColor(green);
    cout << "                  ║  [1] ▶  Bat dau choi             ║\n";
    setColor(yellow);
    cout << "                  ║  [2] ⚙  Chon cap do (Level: " << startingLevel << ")   ║\n";
    setColor(blue);
    cout << "                  ║  [3] ❓  Huong dan choi           ║\n";
    setColor(red);
    cout << "                  ║  [4] ✖  Thoat                    ║\n";
    setColor(white);
    cout << "                  ╚═══════════════════════════════════╝\n";
    cout << "\n";
    setColor(gray);
    cout << "                     Nhap lua chon cua ban...\n";
    setColor(white);

    char c = static_cast<char>(getch());
    if (c == '1') state = PLAYING;
    else if (c == '2') state = SELECTLEVEL;
    else if (c == '3') state = HOWTOPLAY;
    else if (c == '4') state = EXIT;
}

void showHowToPlay(GameState& state) {
    system("cls");
    setColor(cyan);
    cout << "\n\n";
    cout << "         ╔═══════════════════════════════════════════════════════╗\n";
    cout << "         ║                    HUONG DAN CHOI                     ║\n";
    cout << "         ╚═══════════════════════════════════════════════════════╝\n";
    cout << "\n";
    setColor(white);
    cout << "                  ╔═══════════════════════════════════╗\n";
    cout << "                  ║       DIEU KHIEN                 ║\n";
    cout << "                  ╠═══════════════════════════════════╣\n";
    setColor(yellow);
    cout << "                  ║  ↑ / W     : Xoay khoi           ║\n";
    setColor(cyan);
    cout << "                  ║  ← / A     : Di chuyen trai      ║\n";
    cout << "                  ║  → / D     : Di chuyen phai      ║\n";
    setColor(green);
    cout << "                  ║  ↓ / S     : Roi cham            ║\n";
    setColor(purple);
    cout << "                  ║  SPACE     : Roi nhanh           ║\n";
    setColor(orange);
    cout << "                  ║  P         : Tam dung            ║\n";
    setColor(red);
    cout << "                  ║  Q         : Thoat               ║\n";
    setColor(white);
    cout << "                  ╚═══════════════════════════════════╝\n";
    cout << "\n";
    setColor(cyan);
    cout << "                  ╔═══════════════════════════════════╗\n";
    cout << "                  ║  [X] ◄ Quay lai Menu             ║\n";
    cout << "                  ╚═══════════════════════════════════╝\n";
    setColor(white);
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
    cout << "\n\n\n";
    cout << "         ╔═══════════════════════════════════════════════════════╗\n";
    cout << "         ║                   GAME PAUSED ⏸                       ║\n";
    cout << "         ╚═══════════════════════════════════════════════════════╝\n";
    cout << "\n";
    setColor(white);
    cout << "                  ╔═══════════════════════════════════╗\n";
    setColor(green);
    cout << "                  ║  [P] ▶ Tiep tuc choi             ║\n";
    setColor(red);
    cout << "                  ║  [X] ◄ Quay lai Menu             ║\n";
    setColor(white);
    cout << "                  ╚═══════════════════════════════════╝\n";

    while (true) {
        char c = static_cast<char>(getch());
        if (c == 'p' || c == 'P') return true;
        if (c == 'x' || c == 'X') return false;
    }
}

bool gameOverMenu() {
    gotoxy(0, H + 4);
    setColor(red);
    cout << "\n";
    cout << "         ╔═══════════════════════════════════════════════════════╗\n";
    cout << "         ║                    GAME OVER ☠                        ║\n";
    cout << "         ╚═══════════════════════════════════════════════════════╝\n";
    cout << "\n";
    setColor(white);
    cout << "                  ╔═══════════════════════════════════╗\n";
    setColor(yellow);
    cout << "                  ║  Level: " << level;
    if (level < 10) cout << "                          ";
    else cout << "                         ";
    cout << "║\n";
    setColor(cyan);
    cout << "                  ║  Lines: " << linesCleared;
    if (linesCleared < 10) cout << "                         ";
    else if (linesCleared < 100) cout << "                        ";
    else cout << "                       ";
    cout << "║\n";
    setColor(green);
    cout << "                  ║  Score: " << score;
    if (score < 10) cout << "                         ";
    else if (score < 100) cout << "                        ";
    else if (score < 1000) cout << "                       ";
    else if (score < 10000) cout << "                      ";
    else if (score < 100000) cout << "                     ";
    else cout << "                    ";
    cout << "║\n";
    setColor(white);
    cout << "                  ╠═══════════════════════════════════╣\n";
    setColor(green);
    cout << "                  ║  [R] ↻ Choi lai                  ║\n";
    setColor(red);
    cout << "                  ║  [X] ◄ Quay lai Menu             ║\n";
    setColor(white);
    cout << "                  ╚═══════════════════════════════════╝\n";

    while (true) {
        char c = static_cast<char>(getch());
        if (c == 'r' || c == 'R') return true;
        if (c == 'x' || c == 'X') return false;
    }
}

void resetGame() {
    linesCleared = 0;
    level = startingLevel; // Bắt đầu từ level đã chọn
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
    // Thiết lập console để hiển thị ký tự UTF-8 đúng
    SetConsoleOutputCP(65001); // UTF-8
    srand(static_cast<unsigned int>(time(0)));
    hideCursor();

    GameState state = MENU;

    while (state != EXIT) {

        if (state == MENU) {
            showMenu(state);
        }

        else if (state == SELECTLEVEL) {
            selectLevel(state);
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
                if (input.pause) {
                    if (!pauseMenu()) {
                        state = MENU;
                        break;
                    }
                    system("cls");
                    continue; // Vẽ lại màn hình sau khi unpause
                }

                // Update game logic
                updateGame(input);
                
                // Vẽ màn hình mỗi frame
                boardDelBlock(); // Xóa piece thật khỏi board
                drawGhostPiece(); // Vẽ ghost piece
                block2Board(); // Vẽ lại piece thật vào board
                draw();
                clearGhostPiece(); // Xóa ghost piece khỏi board
                
                Sleep(16); // ~60 FPS để mượt
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
