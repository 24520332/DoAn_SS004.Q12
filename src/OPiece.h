#pragma once
#include "Piece.h"

class OPiece : public Piece {
public:
    OPiece();
    
    void rotate(int& x, int& y, char board[H][W]) override;
};