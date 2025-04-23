#ifndef BOARD_H
#define BOARD_H

/* ##### Project Headers ##### */
#include "Piece.hpp"

/* ##### Standard Libraries ##### */
#include <array>

/* Enums */
enum class SquareStatus {Invalid, Empty, Friendly, Enemy};

/* Helper Function - Row, Column to Array Index */
int squareToIndex(int row, int col);
int indexToRow(int index);
int indexToColumn(int index);

class Board {
    private:
    
    public:
        Board();
        ~Board();
    
        std::array<int, 64> whiteAttackBoard;
        std::array<int, 64> blackAttackBoard;
        std::array<Piece *, 64> board; /* PUBLIC for testing and simplifying purposes*/
        
        void clearBoard();
        Piece * getPieceAtIndex(int index) const;
        Piece * createPiece(PieceType type, Color color, int position);
        bool loadFromFEN(const std::string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq");
        SquareStatus getSquareStatus(int fromIndex, int toIndex) const;
        void computeAttackBoards();
        void printAttackBoards();

        void movePiece (int fromIndex, int toIndex);
        
};

#endif