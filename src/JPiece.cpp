#include "JPiece.h"

JPiece::JPiece() : Piece('J') {
    shape[0][1] = 'J';
    shape[1][1] = 'J';
    shape[2][1] = 'J';
    shape[2][0] = 'J';
}

void JPiece::rotate(int& x, int& y, char board[H][W]) {
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