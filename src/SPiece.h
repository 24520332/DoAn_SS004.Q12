#pragma once
#include "Piece.h"

class SPiece : public Piece {
private:
    int wallKickData[4][5][2] = {
        {{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
        {{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
        {{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
        {{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}
    };

public:
    SPiece();

    void rotate(int& x, int& y, char board[H][W]) override;
};