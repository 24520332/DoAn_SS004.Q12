#include "Piece.h"

Piece::Piece(char type) : rotation(0), blockType(type) {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            shape[i][j] = ' ';
}

Piece::~Piece() {}

char Piece::getCell(int i, int j) const {
    return shape[i][j];
}

void Piece::setCell(int i, int j, char c) {
    shape[i][j] = c;
}

int Piece::getRotation() const {
    return rotation;
}

void Piece::setRotation(int r) {
    rotation = r;
}

char Piece::getBlockType() const {
    return blockType;
}

void Piece::rotateShapeMatrix() {
    char temp[4][4] = {};
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            temp[j][3 - i] = shape[i][j];
    
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            shape[i][j] = temp[i][j];
}

bool Piece::isValidPosition(int x, int y, char gameBoard[H][W]) const {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (shape[i][j] != ' ') {
                int tx = x + j;
                int ty = y + i;
                if (tx < 1 || tx >= W - 1 || ty >= H - 1)
                    return false;
                if (ty >= 0 && gameBoard[ty][tx] != ' ')
                    return false;
            }
    return true;
}