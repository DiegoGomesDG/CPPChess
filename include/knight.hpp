#ifndef KNIGHT_H
#define KNIGHT_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

class Knight : public Piece {
    
    public:
        Knight(Color color, int position) : Piece(color, PieceType::Knight, position) {}
};

#endif