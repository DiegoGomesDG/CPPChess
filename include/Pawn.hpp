#ifndef PAWN_H
#define PAWN_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Pawn : public Piece {
    private:
        
    public:
        Pawn(Color color, int position, Board * board, bool hasMoved) : Piece(color, PieceType::Pawn, position, board, hasMoved) {}
        ~Pawn() {}
        Piece * clone(Board* newBoard) const override;
        void computeMoves() override;      
};

#endif