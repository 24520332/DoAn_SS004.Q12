#include "LPiece.h"

LPiece::LPiece() : Piece('L') {
    shape[0][1] = 'L';
    shape[1][1] = 'L';
    shape[2][1] = 'L';
    shape[2][2] = 'L';
}

void LPiece::rotate(int& x, int& y, char gameBoard[H][W]) {
    rotateShapeMatrix();

    for (int k = 0; k < 5; k++) {
        int dx = wallKickData[rotation][k][0];
        int dy = wallKickData[rotation][k][1];

        if (isValidPosition(x + dx, y + dy, gameBoard)) {
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