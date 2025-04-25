#ifndef KNIGHT_H
#define KNIGHT_H

/* ##### Standard Libraries ##### */
#include <array>
#include <vector>

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class Knight : public Piece {
    private:
        static std::array<const int, 8> offsets;
    public:
        Knight(Color color, int position, Board * board) : Piece(color, PieceType::Knight, position, board, true) {}
        ~Knight() {}
        Piece * clone(Board* newBoard) const override;
        void computeMoves() override;
        
};

#endif