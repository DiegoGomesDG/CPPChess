#ifndef BOARD_H
#define BOARD_H

/* ##### Project Headers ##### */
#include "Piece.hpp"
#include "King.hpp"

/* ##### Standard Libraries ##### */
#include <array>

/* ##### Class Forward Declaration ##### */
class ChessGame;

/* ##### Enums ##### */
enum class SquareStatus {Invalid, Empty, Friendly, Enemy};

/* Helper Function - Row, Column to Array Index */
int squareToIndex(int row, int col);
int indexToRow(int index);
int indexToColumn(int index);

class Board {
    private:
        ChessGame * game;
        King * whiteKing;
        King * blackKing;
    public:
        Board(ChessGame * game);
        Board(const Board & original);
        ~Board();
    
        std::array<int, 64> whiteAttackBoard;
        std::array<int, 64> blackAttackBoard;
        std::array<Piece *, 64> board; /* PUBLIC for testing and simplifying purposes*/
        bool validMove;
        bool isChecked;

        void clearBoard();
        Piece* getPieceAt(int index) const {
            return (index >= 0 && index < 64) ? board[index] : nullptr;
        }

        Piece * createPiece(PieceType type, Color color, int position);
        bool loadFromFEN(const std::string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq");
        SquareStatus getSquareStatus(int fromIndex, int toIndex) const;
        void computeAllMoves();
        void computeAttackBoards();
        void validateMovesForPiece(int position);
        bool validateMove(int fromIndex, int toIndex);
        bool movePiece(int fromIndex, int toIndex);
        bool isKingInCheck(Color color);
        
};

#endif