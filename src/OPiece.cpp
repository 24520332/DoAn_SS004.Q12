#include "OPiece.h"

OPiece::OPiece() : Piece('O') {
    shape[1][1] = 'O';
    shape[1][2] = 'O';
    shape[2][1] = 'O';
    shape[2][2] = 'O';
}

void OPiece::rotate(int& x, int& y, char board[H][W]) {
    // O piece không xoay
}