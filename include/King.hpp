#ifndef KING_H
#define KING_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Class Forward Declaration ##### */
class Board;

class King : public Piece {
public:
    King(Color color, int position, Board * board, bool hasMoved);
    ~King() {}
    Piece * clone(Board* newBoard) const override;

    void setKingSideCastleRight(bool state) { mKingSideCastle = state; }
    void setQueenSideCastleRight(bool state) { mQueenSideCastle = state; }
    void setCheck(bool state) { mInCheck = state; }
    
    bool isChecked() const { return mInCheck; }
    bool hasCastleRights() const { return mKingSideCastle || mQueenSideCastle; }

    void computeMoves() override;
    void computeCastling();

private:
    bool mKingSideCastle;
    bool mQueenSideCastle;
    bool mInCheck;
    static std::array<const int, 8> mOffsets;
};

#endif