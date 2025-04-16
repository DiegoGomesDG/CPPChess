#ifndef BISHOP_H
#define BISHOP_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

class Bishop : public Piece {

    public:
        Bishop(Color color, int position) : Piece(color, PieceType::Bishop, position) {}
};

#endif