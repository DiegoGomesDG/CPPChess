#ifndef KING_H
#define KING_H

#include "piece.hpp"

class King : public Piece {
    private:
        bool kingSideCastle;
        bool queenSideCastle;
        bool inCheck;
        bool hasMoved;
    public:
        King(Color color, int position) : Piece(color, PieceType::King, position) {}
};

#endif