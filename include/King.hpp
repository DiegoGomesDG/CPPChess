#ifndef KING_H
#define KING_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class King : public Piece {
    private:
        bool kingSideCastle;
        bool queenSideCastle;
        bool inCheck;
        bool hasMoved;
        static std::array<const int, 8> offsets;
    public:
        King(Color color, int position, Board * board);
        ~King() {}
        void setKingSideCastleRight(bool state) {kingSideCastle = state;}
        void setQueenSideCastleRight(bool state) {queenSideCastle = state;}
        
        void computeValidMoves() override;
};

#endif