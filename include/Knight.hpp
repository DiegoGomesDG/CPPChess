#ifndef KNIGHT_H
#define KNIGHT_H

/* ##### Standard Libraries ##### */
#include <array>

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Knight : public Piece {
public:
    Knight(Color color, int position, Board * board) : Piece(color, PieceType::Knight, position, board, true) {}
    ~Knight() {}
    Piece * clone(Board* newBoard) const override;
    void computeMoves() override;

private:
    static std::array<const int, 8> mOffsets;
};

#endif