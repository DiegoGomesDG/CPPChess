#ifndef ROOK_H
#define ROOK_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Rook : public Piece {
public:
    Rook(Color color, int position, Board * board) : Piece(color, PieceType::Rook, position, board, true) {}
    ~Rook() {}
    void computeMoves() override;
    Piece * clone(Board* newBoard) const override;
    
private:
    static std::array<const int, 4> mOffsets;
};

#endif