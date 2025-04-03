#ifndef BISHOP_H
#define BISHOP_H

#include "piece.hpp"

class Bishop : public Piece {

    public:
        Bishop(Color color, int position) : Piece(color, PieceType::Bishop, position) {}
};

#endif