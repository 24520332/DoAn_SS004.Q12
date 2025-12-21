#pragma once
#include "Piece.h"

class TPiece : public Piece {
private:
    int wallKickData[4][5][2] = {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

public:
    TPiece();

    void rotate(int& x, int& y, char gameBoard[H][W]) override;
};