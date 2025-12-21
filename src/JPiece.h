#pragma once
#include "Piece.h"

class JPiece : public Piece {
private:
    int wallKick[4][5][2] = {
        {{0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2}},
        {{0,0}, {1,0}, {1,-1}, {0,2}, {1,2}},
        {{0,0}, {1,0}, {1,1}, {0,-2}, {1,-2}},
        {{0,0}, {-1,0}, {-1,-1}, {0,2}, {-1,2}}
    };

public:
    JPiece();

    void rotate(int& x, int& y, char gameBoard[H][W]) override;
};