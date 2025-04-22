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
    public:
        King(Color color, int position);
        ~King() {}
        void setKingSideCastleRight(bool state) {kingSideCastle = state;}
        void setQueenSideCastleRight(bool state) {queenSideCastle = state;}
        
        void possibleMoves(const Board& board) override {}
};

#endif