#ifndef KING_H
#define KING_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

class King : public Piece {
    private:
        bool kingSideCastle;
        bool queenSideCastle;
        bool inCheck;
        bool hasMoved;
    public:
        King(Color color, int position);
        void setKingSideCastleRight(bool state);
        void setQueenSideCastleRight(bool state);
};

#endif