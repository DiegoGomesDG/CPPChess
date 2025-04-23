#ifndef PAWN_H
#define PAWN_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Pawn : public Piece {
    private:
        bool doublePush; // Ensures that only in the first move you can move 2 squares up/down
    public:
        std::vector<int> forwardMoves;
        Pawn(Color color, int position, Board * board) : Piece(color, PieceType::Pawn, position, board), doublePush(false) {}
        ~Pawn() {}
        void setDoublePush(bool state) {doublePush = state;}
        void computeValidMoves(const Board& board) override;
};

#endif