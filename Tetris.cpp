#include <iostream>
#include <conio.h>
#include <windows.h>
using namespace std;
#define H 20
#define W 15
char board[H][W] = {} ;
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

// Wall kick data cho các khối J, L, S, T, Z
int wallKickCW_Normal[4][5][2] = {
    // 0 -> 1
    {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}},
    // 1 -> 2
    {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}},
    // 2 -> 3
    {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}},
    // 3 -> 0
    {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}}
};

// Wall kick data đặc biệt cho khối I
int wallKickCW_I[4][5][2] = {
    {{0,0}, {-2,0}, {1,0}, {-2,-1}, {1,2}},
    {{0,0}, {-1,0}, {2,0}, {-1,2}, {2,-1}},
    {{0,0}, {2,0}, {-1,0}, {2,1}, {-1,-2}},
    {{0,0}, {1,0}, {-2,0}, {1,-2}, {-2,1}}
};

char currentBlock[4][4]; // Block đang rơi



int rotation = 0;  // 0, 1, 2, 3 cho 4 hướng xoay

int x=4,y=0,b=1;
void gotoxy(int x, int y) {
    COORD c = {x, y};
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
            cout<<board[i][j];
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

}

int main()
{
    srand(time(0));
    b = rand() % 7;
    spawnBlock();
    
    rotation = 0; // Khởi tạo rotation
    system("cls");
    initBoard();
    
    while (true){
        boardDelBlock();
        if (kbhit()){
            char c = getch();
            if (c=='a' && canMove(-1,0)) x--;
            if (c=='d' && canMove(1,0)) x++;
            if (c=='s' && canMove(0,1)) y++; // Sửa 'x' thành 's' cho quen
            if (c=='w') rotateBlock();      // 'w' để xoay CW
            if (c=='q') break;
        }
        if (canMove(0,1)) y++;
        else {
            block2Board();
            removeLine();
            x = 5; y = 0; 
            b = rand() % 7;
            rotation = 0; // Reset rotation cho khối mới

            spawnBlock();
        }
        block2Board();
        draw();
        Sleep(200);
    }
    return 0;
}