#ifndef QUEEN_H
#define QUEEN_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Queen : public Piece {
public:
    Queen(Color color, int position, Board * board) : Piece(color, PieceType::Queen, position, board, true) {}  
    ~Queen() {}
    void computeMoves() override;
    Piece * clone(Board* newBoard) const override;

private:
    static std::array<const int, 8> mOffsets;
};

#endif