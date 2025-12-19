#include "IPiece.h"

IPiece::IPiece() : Piece('I') {
    shape[0][1] = 'I';
    shape[1][1] = 'I';
    shape[2][1] = 'I';
    shape[3][1] = 'I';
}

void IPiece::rotate(int& x, int& y, char board[H][W]) {
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