#ifndef BISHOP_H
#define BISHOP_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Bishop : public Piece {
    private:
        static std::array<const int, 4> offsets;
    public:
        Bishop(Color color, int position, Board * board) : Piece(color, PieceType::Bishop, position, board) {}
        ~Bishop() {}
        void computeValidMoves() override;
};

#endif