#pragma once
#include "Piece.h"

class IPiece : public Piece {
    
private:
    int wallKickData[4][5][2] = {
        {{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
        {{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
        {{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
        {{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}
    };
    
public:
    IPiece();
    
    void rotate(int& x, int& y, char gameBoard[H][W]) override;
};