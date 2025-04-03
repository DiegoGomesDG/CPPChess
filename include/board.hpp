#ifndef GAME_H
#define GAME_H

#include "graphics.hpp"
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
        Piece* createPiece(PieceType type, Color color, int position);
        bool loadFromFEN(const std::string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
};


#endif