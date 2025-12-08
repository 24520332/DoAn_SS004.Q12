#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime> //(VuQuan) Sử dụng srand(time(0))
using namespace std;
#define H 20
#define W 15
char board[H][W] = {} ;
char currentBlock[4][4] = {};
char blockTemplates[][4][4] = {
        {{' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '},
         {' ','I',' ',' '}},
        {{' ',' ',' ',' '},
         {' ','O','O',' '},
         {' ','O','O',' '},
         {' ',' ',' ',' '}},

        {{' ',' ',' ',' '},
         {' ','T',' ',' '},
         {'T','T','T',' '},
         {' ',' ',' ',' '}},

        {{' ',' ',' ',' '},
         {' ','S','S',' '},
         {'S','S',' ',' '},
         {' ',' ',' ',' '}},

        {{' ',' ',' ',' '},
         {'Z','Z',' ',' '},
         {' ','Z','Z',' '},
         {' ',' ',' ',' '}},

        {{' ',' ',' ',' '},
         {'J',' ',' ',' '},
         {'J','J','J',' '},
         {' ',' ',' ',' '}},

        {{' ',' ',' ',' '},
         {' ',' ','L',' '},
         {'L','L','L',' '},
         {' ',' ',' ',' '}}
};

int wallKickCW_I[4][5][2] = {
    // State 0->1
    {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
    // State 1->2
    {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
    // State 2->3
    {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
    // State 3->0
    {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}
};

// Wall kick data cho các block thông thường (J, L, S, T, Z)
int wallKickCW_Normal[4][5][2] = {
    // State 0->1
    {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    // State 1->2
    {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    // State 2->3
    {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    // State 3->0
    {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
};

// --------------
// --- Đồ họa ---
// --------------

// Màu sắc
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
    // Xem thêm tại bảng màu "Windows Console Color Table"
};

// Đổi màu chữ cho console Windows
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// Chuyển thành block màu
char ColorBlock(char c, int color) {
    setColor(color);
    return '\xDB';
}

char blockChar(char c) {
    switch(c) {
        case 'I': 
            return ColorBlock(c, cyan);
        case 'O': 
            return ColorBlock(c, yellow);
        case 'T': 
            return ColorBlock(c, purple);
        case 'S': 
            return ColorBlock(c, green);
        case 'Z':
            return ColorBlock(c, red);
        case 'J': 
            return ColorBlock(c, blue);
        case 'L': 
            return ColorBlock(c, orange);
        case '#':
            return ColorBlock(c, gray);
        default: 
            return ' ';
    }
}
//---------------

int rotation = 0;  // 0, 1, 2, 3 cho 4 hướng xoay
//Thêm biến toàn cục cho dòng và level
int linesCleared = 0;
int level = 1;

int x=4,y=0,b=1;
int FallSpeed = 200; //(VuQuan) Thêm biến tốc độ rơi
void gotoxy(int x, int y) {
    COORD c = {(SHORT)x, (SHORT)y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void spawnBlock() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            currentBlock[i][j] = blockTemplates[b][i][j];
}

void boardDelBlock(){
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (currentBlock[i][j] != ' ') {
                int tx = x + j;
                int ty = y + i;
                // kiểm tra trong biên trước khi gán
                if (ty >= 0 && ty < H && tx >= 0 && tx < W)
                    board[ty][tx] = ' ';
            }
        }
    }
}

void block2Board(){
    for (int i = 0 ; i < 4 ; i++){
        for (int j = 0 ; j < 4 ; j++){
            if (currentBlock[i][j] != ' ') {
                int tx = x + j;
                int ty = y + i;
                if (ty >= 0 && ty < H && tx >= 0 && tx < W)
                    board[ty][tx] = currentBlock[i][j];
            }
        }
    }

}
void initBoard(){
    for (int i = 0 ; i < H ; i++)
        for (int j = 0 ; j < W ; j++)
            if ((i==H-1) || (j==0) || (j == W-1)) board[i][j] = '#';
            else board[i][j] = ' ';
}

void draw(){
    gotoxy(0,0);
    for (int i = 0 ; i < H ; i++, cout<<endl)
        for (int j = 0 ; j < W ; j++)
        {
            cout<<blockChar(board[i][j])<<blockChar(board[i][j]);
            setColor(white); //Đổi lại màu trắng sau khi in block
        }


    //Hiện level và dòng đã cleard dưới bảng
    cout << "Level: " << level << " | Lines: " << linesCleared << endl;
}

bool canMove(int dx, int dy){
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (currentBlock[i][j] != ' ') {
                int tx = x + j + dx;
                int ty = y + i + dy;

                if (tx < 1 || tx >= W - 1 || ty >= H - 1)
                    return false;
                if (board[ty][tx] != ' ')
                    return false;
            }
    return true;
}

void rotateBlock() {
    if (b == 1) return; // O block không xoay

    char temp[4][4] = {};

    // Xoay chiều kim đồng hồ trong bộ nhớ
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[j][3 - i] = currentBlock[i][j];

    // Chọn bảng wall kick
    int (*kick)[5][2];
    kick = (b == 0) ? wallKickCW_I : wallKickCW_Normal;

    // Thử từng wall kick
    for (int k = 0; k < 5; k++) {
        int dx = kick[rotation][k][0];
        int dy = kick[rotation][k][1];

        bool valid = true;

        for (int i = 0; i < 4 && valid; i++)
            for (int j = 0; j < 4; j++)
                if (temp[i][j] != ' ') {
                    int tx = x + j + dx;
                    int ty = y + i + dy;

                    if (tx < 1 || tx >= W-1 ||
                        ty >= H-1 ||
                        board[ty][tx] != ' ') {
                        valid = false;
                        break;
                    }
                }

        if (valid) {
            x += dx;
            y += dy;

            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    currentBlock[i][j] = temp[i][j];

            rotation = (rotation + 1) % 4;
            return;
        }
    }
}

void removeLine(){
    int cleared = 0; //số dòng xóa
    int dest = H - 2; //bắt đầu ghi từ hàng H-2
    for (int src = H - 2; src >= 0; src--) {
        bool full = true;
        //Kiểm tra hàng src có đầy không (cột 1 đến W-2)
        for (int j = 1; j < W - 1; j++) {
            if (board[src][j] == ' ') {
                full = false;
                break;
            }
        }

        if (!full) {
            //Sao chép hàng src vào dest
            for (int j = 1; j < W-1; j++) {
                board[dest][j] = board[src][j];
            }
            dest--;
        } else {
            cleared++; //Đếm dòng đầy
        }
    }

    //Điền vào hàng trên cùng (từ 0 đến dest) bằng ' '
    for (int i = 0; i <= dest; i++) {
        for (int j = 1; j < W-1; j++) {
            board[i][j] = ' ';
        }
    }

    //Cập nhập số dòng đã xóa
    linesCleared += cleared;
}


int main()
{
    srand(time(0));
    b = rand() % 7;
    spawnBlock();

    rotation = 0;
    system("cls");
    initBoard();
    
    bool gameOver = false;
    
    while (!gameOver){
        boardDelBlock();
        if (kbhit()){ 
            int c = getch();
            if (c == 0 || c == 224) {
                c = getch(); 
                // Arrow keys
                if (c==75 && canMove(-1,0)) x--;
                if (c==77 && canMove(1,0)) x++;
                if (c==80 && canMove(0,1)) y++;
                if (c==72) rotateBlock();  // BẬT ROTATE
            } else {
                // Normal keys (A, S, D, W, Q)
                if ((c=='a' || c=='A') && canMove(-1,0)) x--;
                if ((c=='d' || c=='D') && canMove(1,0)) x++;
                if ((c=='s' || c=='S') && canMove(0,1)) y++;
                if (c=='w' || c=='W') rotateBlock();
                if (c=='q' || c=='Q') {
                    gameOver = true;
                }
            }
            // XÓA HẾT KÝ TỰ THỪA TRONG BUFFER
            while (kbhit()) getch();
        }

        if (canMove(0,1)) y++;
        else {
            block2Board();
            removeLine();
            level = linesCleared/10+1;
            x = 5; y = 0;
            b = rand() % 7;
            rotation = 0;

            spawnBlock();
            if (!canMove(0, 0)) {
                gameOver = true;
            }
        }
        block2Board();
        draw();
        int speed = max(50, 200 - (level - 1) * 20);
        Sleep(speed);
    }
    
    gotoxy(0, H + 3);
    setColor(red);
    cout << "\n*** GAME OVER ***\n";
    setColor(white);
    cout << "Final - Level: " << level << " | Lines: " << linesCleared << endl;
    cout << "Press any key to exit...";
    getch();

    return 0;
}
