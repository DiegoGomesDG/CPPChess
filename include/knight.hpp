#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.hpp"

class Knight : public Piece {
    
    public:
        Knight(Color color, int position) : Piece(color, PieceType::Knight, position) {}
};

#endif