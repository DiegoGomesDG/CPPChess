#ifndef BOARD_H
#define BOARD_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Standard Libraries ##### */
#include <iostream>
#include <array>

/* Helper Function - Row, Column to Array Index */
int squareToIndex(int row, int col);
int indexToRow(int index);
int indexToColumn(int index);

class Board {
    private:
        std::array<int, 64> whiteOccupation;
        std::array<int, 64> blackOccupation;
        std::array<int, 64> whiteAttackBoard;
        std::array<int, 64> blackAttackBoard;
    public:
        std::array<Piece *, 64> board; /* PUBLIC for testing and simplifying purposes*/
        Board();
        ~Board();
        void clearBoard();
        Piece * getPieceAtIndex(int index) const;
        Piece * createPiece(PieceType type, Color color, int position);
        bool loadFromFEN(const std::string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w");
};

#endif