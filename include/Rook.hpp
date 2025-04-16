#ifndef ROOK_H
#define ROOK_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

class Rook : public Piece {
    private:
        bool hasMoved;
    public:
        Rook(Color color, int position) : Piece(color, PieceType::Rook, position) {}
};

#endif