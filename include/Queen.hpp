#ifndef QUEEN_H
#define QUEEN_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Queen : public Piece {
    private:
        static std::array<const int, 8> offsets;
    public:
        Queen(Color color, int position, Board * board) : Piece(color, PieceType::Queen, position, board) {}  
        ~Queen() {}
        void computeValidMoves(const Board& board) override;
};

#endif