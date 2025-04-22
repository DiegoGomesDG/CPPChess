#ifndef QUEEN_H
#define QUEEN_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Queen : public Piece {
    private:
    public:
        Queen(Color color, int position) : Piece(color, PieceType::Queen, position) {}  
        ~Queen() {}
        void possibleMoves(const Board& board) override {}
};

#endif