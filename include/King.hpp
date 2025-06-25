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
        static std::array<const int, 8> offsets;
    public:
        King(Color color, int position, Board * board, bool hasMoved);
        ~King() {}
        Piece * clone(Board* newBoard) const override;

        void setKingSideCastleRight(bool state) {kingSideCastle = state;}
        void setQueenSideCastleRight(bool state) {queenSideCastle = state;}
        void setCheck(bool state) {inCheck = state;}
        
        bool isChecked() const {return inCheck;}
        bool hasCastleRights() const {return kingSideCastle || queenSideCastle;}

        void computeMoves() override;
        void computeCastling();

};

#endif