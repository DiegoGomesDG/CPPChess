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
        Knight(Color color, int position) : Piece(color, PieceType::Knight, position) {}
        ~Knight() {}
        void possibleMoves(const Board& board) override;
};

#endif