#pragma once
#include "Board.h"

class Piece {
protected:
    char shape[4][4];
    int rotation;
    char blockType;
    
public:
    Piece(char type);
    virtual ~Piece();

    // Hàm ảo để xoay - mỗi loại piece có cách xoay riêng
    virtual void rotate(int& x, int& y, char gameBoard[H][W]) = 0;

    // Getter cho shape
    char getCell(int i, int j) const;

    void setCell(int i, int j, char c);

    int getRotation() const;

    void setRotation(int r);

    char getBlockType() const;
    
    // Hàm xoay cơ bản (clockwise)
    void rotateShapeMatrix();

    // Kiểm tra vị trí hợp lệ
    bool isValidPosition(int x, int y, char gameBoard[H][W]) const;
};