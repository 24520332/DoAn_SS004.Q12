#define _CRT_NONSTDC_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
using namespace std;

#define H 20
#define W 15

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
// --- CLASS PIECE ---
// ----------------------

class Piece {
protected:
    char shape[4][4];
    int rotation;
    char blockType;
    
public:
    Piece(char type) : rotation(0), blockType(type) {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = ' ';
    }
    
    virtual ~Piece() {}
    
    // Hàm ảo để xoay - mỗi loại piece có cách xoay riêng
    virtual void rotate(int& x, int& y, char board[H][W]) = 0;
    
    // Getter cho shape
    char getCell(int i, int j) const {
        return shape[i][j];
    }
    
    void setCell(int i, int j, char c) {
        shape[i][j] = c;
    }
    
    int getRotation() const {
        return rotation;
    }
    
    void setRotation(int r) {
        rotation = r;
    }
    
    char getBlockType() const {
        return blockType;
    }
    
    // Hàm xoay cơ bản (clockwise)
    void rotateShapeMatrix() {
        char temp[4][4] = {};
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                temp[j][3 - i] = shape[i][j];
        
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                shape[i][j] = temp[i][j];
    }
    
    // Kiểm tra vị trí hợp lệ
    bool isValidPosition(int x, int y, char board[H][W]) const {
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 4; j++)
                if (shape[i][j] != ' ') {
                    int tx = x + j;
                    int ty = y + i;
                    if (tx < 1 || tx >= W - 1 || ty >= H - 1)
                        return false;
                    if (ty >= 0 && board[ty][tx] != ' ')
                        return false;
                }
        return true;
    }
};

// --- I Piece ---
class IPiece : public Piece {
    
private:
    int wallKickData[4][5][2] = {
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}
    };
    
public:
    IPiece() : Piece('I') {
        shape[0][1] = 'I';
        shape[1][1] = 'I';
        shape[2][1] = 'I';
        shape[3][1] = 'I';
    }
    
    void rotate(int& x, int& y, char board[H][W]) override {
        rotateShapeMatrix();
        
        // Thử wall kick
        for (int k = 0; k < 5; k++) {
            int dx = wallKickData[rotation][k][0];
            int dy = wallKickData[rotation][k][1];
            
            if (isValidPosition(x + dx, y + dy, board)) {
                x += dx;
                y += dy;
                rotation = (rotation + 1) % 4;
                return;
            }
        }
        
        // Nếu không xoay được, xoay ngược lại
        for (int i = 0; i < 3; i++)
            rotateShapeMatrix();
    }
};

// --- O Piece (không xoay) ---
class OPiece : public Piece {
public:
    OPiece() : Piece('O') {
        shape[1][1] = 'O';
        shape[1][2] = 'O';
        shape[2][1] = 'O';
        shape[2][2] = 'O';
    }
    
    void rotate(int& x, int& y, char board[H][W]) override {
        // O piece không xoay
    }
};

// --- T Piece ---
class TPiece : public Piece {
private:
    int wallKickData[4][5][2] = {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

public:
    TPiece() : Piece('T') {
        shape[0][1] = 'T';
        shape[1][0] = 'T';
        shape[1][1] = 'T';
        shape[1][2] = 'T';
    }

    void rotate(int& x, int& y, char board[H][W]) override {
        rotateShapeMatrix();

        for (int k = 0; k < 5; k++) {
            int dx = wallKickData[rotation][k][0];
            int dy = wallKickData[rotation][k][1];

            if (isValidPosition(x + dx, y + dy, board)) {
                x += dx;
                y += dy;
                rotation = (rotation + 1) % 4;
                return;
            }
        }

        for (int i = 0; i < 3; i++)
            rotateShapeMatrix();
    }
};

// --- S Piece ---
class SPiece : public Piece {
private:
    int wallKickData[4][5][2] = {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

public:
    SPiece() : Piece('S') {
        shape[1][1] = 'S';
        shape[1][2] = 'S';
        shape[2][0] = 'S';
        shape[2][1] = 'S';
    }

    void rotate(int& x, int& y, char board[H][W]) override {
        rotateShapeMatrix();

        for (int k = 0; k < 5; k++) {
            int dx = wallKickData[rotation][k][0];
            int dy = wallKickData[rotation][k][1];

            if (isValidPosition(x + dx, y + dy, board)) {
                x += dx;
                y += dy;
                rotation = (rotation + 1) % 4;
                return;
            }
        }

        for (int i = 0; i < 3; i++)
            rotateShapeMatrix();
    }
};

// --- Z Piece ---
class ZPiece : public Piece {
private:

    int wallKickData[4][5][2] = {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

public:
    ZPiece() : Piece('Z') {
        shape[1][0] = 'Z';
        shape[1][1] = 'Z';
        shape[2][1] = 'Z';
        shape[2][2] = 'Z';
    }

    void rotate(int& x, int& y, char board[H][W]) override {



        rotateShapeMatrix();

        for (int k = 0; k < 5; k++) {
            int dx = wallKickData[rotation][k][0];
            int dy = wallKickData[rotation][k][1];

            if (isValidPosition(x + dx, y + dy, board)) {
                x += dx;
                y += dy;
                rotation = (rotation + 1) % 4;
                return;
            }
        }

        for (int i = 0; i < 3; i++)
            rotateShapeMatrix();
    }
};

// --- J Piece ---
class JPiece : public Piece {
private:
    int wallKick[4][5][2] = {
        {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}},
        {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}},
        {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}},
        {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}}
    };

public:
    JPiece() : Piece('J') {
        shape[0][1] = 'J';
        shape[1][1] = 'J';
        shape[2][1] = 'J';
        shape[2][0] = 'J';
    }

    void rotate(int& x, int& y, char board[H][W]) override {
        rotateShapeMatrix();

        for (int k = 0; k < 5; k++) {
            int dx = wallKick[rotation][k][0];
            int dy = wallKick[rotation][k][1];

            if (isValidPosition(x + dx, y + dy, board)) {
                x += dx;
                y += dy;
                rotation = (rotation + 1) % 4;
                return;
            }
        }

        // rollback
        for (int i = 0; i < 3; i++)
            rotateShapeMatrix();
    }
};

// --- L Piece ---
class LPiece : public Piece {
private:
    int wallKickData[4][5][2] = {
        {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}},
        {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}},
        {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}},
        {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}}
    };

public:
    LPiece() : Piece('L') {
        shape[0][1] = 'L';
        shape[1][1] = 'L';
        shape[2][1] = 'L';
        shape[2][2] = 'L';
    }

    void rotate(int& x, int& y, char board[H][W]) override {
        rotateShapeMatrix();

        for (int k = 0; k < 5; k++) {
            int dx = wallKickData[rotation][k][0];
            int dy = wallKickData[rotation][k][1];

            if (isValidPosition(x + dx, y + dy, board)) {
                x += dx;
                y += dy;
                rotation = (rotation + 1) % 4;
                return;
            }
        }

        // rollback
        for (int i = 0; i < 3; i++)
            rotateShapeMatrix();
    }
};

// ----------------------
// --- GAME FUNCTIONS ---
// ----------------------

int linesCleared = 0;
int level = 1;
int x = 4, y = 0;
int initialSpeed = 200; // (VQ) Tốc độ mặc định
Piece* currentPiece = nullptr;
Piece* nextPiece = nullptr;

void gotoxy(int x, int y) {
    COORD c = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

// (VQ) Hàm hiển thị Menu 
int showMenu() {
    int choice = 0;
    string options[] = { " EASY ", " MEDIUM ", " HARD ", " EXIT " };
    int numOptions = 4;

    while (true) {
        system("cls");
        setColor(yellow);
        cout << "\n\n";
        cout << "   ======================\n";
        cout << "   |       TETRIS       |\n";
        cout << "   ======================\n\n";

        setColor(white);
        cout << "      Select The Level To Play:\n\n";

        for (int i = 0; i < numOptions; i++) {
            if (i == choice) {
                setColor(cyan); //(VQ) Màu cho nút chọn
                cout << "     >> [ " << options[i] << " ] <<" << endl;
            }
            else {
                setColor(gray);
                cout << "          " << options[i] << endl;
            }
        }

        setColor(white);
        cout << "\n   Use the arrow keys to move and the Enter key to select!";

        int key = getch();
        if (key == 224) { //(VQ) Phím mũi tên
            key = getch();
            if (key == 72) choice = (choice - 1 + numOptions) % numOptions; //(VQ) Lên
            if (key == 80) choice = (choice + 1) % numOptions;              //(VQ) Xuống
        }
        else if (key == 13) { //(VQ) Enter
            return choice;
        }
    }
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
    cout << "Press P to pause the game" << endl;
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

// ----------------------
// --- MENU SCREEN ---
// ----------------------
//Thêm các trạng thái game
enum GameState { MENU, PLAYING, HOWTOPLAY, EXIT };

//Menu
void showMenu(GameState& state) {
    system("cls");
    setColor(cyan);
    cout << "======== TETRIS ========\n\n";
    setColor(white);
    cout << "1. Start Game\n";
    cout << "2. How to Play\n";
    cout << "3. Exit\n\n";

    char c = getch();
    if (c == '1') state = PLAYING;
    else if (c == '2') state = HOWTOPLAY;
    else if (c == '3') state = EXIT;
}

//Hướng dẫn (How to Play)
void showHowToPlay(GameState& state) {
    system("cls");
    setColor(yellow);
    cout << "====== HOW TO PLAY ======\n\n";
    setColor(white);
    cout << "A/D: Move left/right\n";
    cout << "W: Rotate\n";
    cout << "S: Soft drop\n";
    cout << "P: Pause the game\n";

    cout << "\nPress X to return to Menu\n";
    while (true) {
        char c = getch();
        if (c == 'x' || c == 'X') {
            state = MENU;
            return;
        }
    }
}

//Dừng màn chơi và menu Pause
bool pauseMenu() {
    system("cls");
    setColor(yellow);
    cout << "=== GAME PAUSED ===\n\n";
    setColor(white);
    cout << "P - Continue Game\n";
    cout << "X - Return to Menu\n";

    while (true) {
        char c = getch();
        if (c == 'p' || c == 'P') return true;   //Tiếp tục (Continue Game)
        if (c == 'x' || c == 'X') return false;  //Trở về/Thoát màn chơi (Return to Menu)
    }
}

//Thông báo khi thua 
bool gameOverMenu() {
    gotoxy(0, H + 2);
    setColor(red);
    cout << "\n*** GAME OVER ***\n";
    setColor(white);
    cout << "R - Play Again\n";
    cout << "X - Return to Menu\n";

    while (true) {
        char c = getch();
        if (c == 'r' || c == 'R') return true;   // Chơi lại
        if (c == 'x' || c == 'X') return false;  // Trở về
    }
}

//Chơi lại (Play Again)
void resetGame() {
    linesCleared = 0;
    level = 1;
    initBoard();

    if (currentPiece) delete currentPiece;
    if (nextPiece) delete nextPiece;

    currentPiece = createRandomPiece();
    nextPiece = createRandomPiece();


    x = W / 2 - 2;
    y = 0;
}


/*int main() {
    srand(time(0));

    //(VQ) Hiển thị menu
    int mode = showMenu();
    if (mode == 3) return 0; //(VQ) Thoát game

    //(VQ) Thiết lập tốc độ dựa trên lựa chọn
    if (mode == 0) initialSpeed = 400; //(VQ) Dễ
    if (mode == 1) initialSpeed = 200; //(VQ) Vừa
    if (mode == 2) initialSpeed = 80;  //(VQ) Khó
    
    system("cls");
    initBoard();
    
    currentPiece = createRandomPiece();
    x = 5;  // SỬA: Spawn ở giữa bảng (W=15, nên x=5 hợp lý hơn x=4)
    y = 0;
    
    bool gameOver = false;
    
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
        
        //(VQ) Tính toán tốc độ
        int speed = max(30, initialSpeed - (level - 1) * 15);
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
}*/

//Thay đổi hàm main cho các hàm mới
int main() {
    srand(time(0));

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

            bool gameOver = false;

            while (!gameOver) {
                boardDelBlock();

                if (kbhit()) {
                    int c = getch();

                    if (c == 'p' || c == 'P') {
                        if (!pauseMenu()) {
                            state = MENU;
                            break;
                        }
                        system("cls");
                    }

                    if (c == 0 || c == 224) {
                        c = getch();
                        if (c == 75 && canMove(-1, 0)) x--;
                        if (c == 77 && canMove(1, 0)) x++;
                        if (c == 80 && canMove(0, 1)) y++;
                        if (c == 72) currentPiece->rotate(x, y, board);
                    }
                    else {
                        if ((c == 'a' || c == 'A') && canMove(-1, 0)) x--;
                        if ((c == 'd' || c == 'D') && canMove(1, 0)) x++;
                        if ((c == 's' || c == 'S') && canMove(0, 1)) y++;
                        if (c == 'w' || c == 'W') currentPiece->rotate(x, y, board);
                    }

                    while (kbhit()) getch();
                }

                if (canMove(0, 1)) {
                    y++;
                }
                else {
                    block2Board();
                    removeLine();
                    level = linesCleared / 10 + 1;

                    delete currentPiece;
                    currentPiece = nextPiece;
                    nextPiece = createRandomPiece();
                    x = W / 2 - 2;
                    y = 0;

                    if (!canMove(0, 0)) {
                        gameOver = true;
                    }
                }

                block2Board();
                draw();

                int speed = max(50, 200 - (level - 1) * 20);
                Sleep(speed);
            }

            if (state == PLAYING) {
                bool replay = gameOverMenu();
                if (!replay) state = MENU;
            }
        }
    }

    if (currentPiece) delete currentPiece;
    if (nextPiece) delete nextPiece;
    return 0;
}
