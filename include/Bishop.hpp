#ifndef BISHOP_H
#define BISHOP_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Bishop : public Piece {
public:
    Bishop(Color color, int position, Board * board) : Piece(color, PieceType::Bishop, position, board, true) {}
    ~Bishop() {}
    Piece * clone(Board* newBoard) const override;
    void computeMoves() override;
    
private:
    static std::array<const int, 4> mOffsets;
};

#endif