#include "TPiece.h"

TPiece::TPiece() : Piece('T') {
    shape[0][1] = 'T';
    shape[1][0] = 'T';
    shape[1][1] = 'T';
    shape[1][2] = 'T';
}

void TPiece::rotate(int& x, int& y, char board[H][W]) {
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