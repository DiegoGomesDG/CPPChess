#ifndef BOARD_H
#define BOARD_H

#include "piece.hpp"
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
    public:
        std::array<Piece *, 64> board;
        Board();
        ~Board();
        void clearBoard();
        Piece* createPiece(PieceType type, Color color, int position);
        bool loadFromFEN(const std::string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
};


#endif