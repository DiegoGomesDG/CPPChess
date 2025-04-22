#ifndef PAWN_H
#define PAWN_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Pawn : public Piece {
    private:
        bool hasMoved; // Ensures that only in the first move you can move 2 squares up/down
    public:
        Pawn(Color color, int position) : Piece(color, PieceType::Pawn, position), hasMoved(false) {}
        ~Pawn() {}
        void setMoveRight(bool state) {hasMoved = state;}
        void possibleMoves(const Board& board) override {}
};

#endif