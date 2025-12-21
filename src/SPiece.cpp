#include "SPiece.h"

SPiece::SPiece() : Piece('S') {
    shape[1][1] = 'S';
    shape[1][2] = 'S';
    shape[2][0] = 'S';
    shape[2][1] = 'S';
}

void SPiece::rotate(int& x, int& y, char gameBoard[H][W]) {
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

    for (int i = 0; i < 3; i++)
        rotateShapeMatrix();
}