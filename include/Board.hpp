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

class Board {
    private:
        ChessGame * game;
        King * whiteKing;
        King * blackKing;
        int enPassantIndex;
    public:
        Board(ChessGame * game); /* Constructor */
        Board(const Board & original); /* Copy Constructor */
        ~Board(); /* Destructor */
    
        std::array<int, 64> whiteAttackBoard;
        std::array<int, 64> blackAttackBoard;
        std::array<Piece *, 64> board; /* PUBLIC for simplifying purposes*/
        int castlingOffset; /* just for the animation */

        /* Static Methods - Convertion of (row, col) to index and back */
        static int squareToIndex(int row, int col);
        static int indexToRow(int index);
        static int indexToColumn(int index);

        /* Static Methods - Conversion of an index to Algebraic Notation (e.g. e4, f2, etc) and the otherway `*/
        static int algebraicToIndex(const std::string & notation);
        static std::string indexToAlgebraic(int index);

        /* Clear the board*/
        void clearBoard();

        /* Piece creation and board initialization */
        Piece * createPiece(PieceType type, Color color, int position);
        bool loadFromFEN(const std::string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -");

        /* Helpers for move computation */
        static bool isValidIndex(int index);
        SquareStatus getSquareStatus(int fromIndex, int toIndex) const;

        /* Compute moves and attack boards*/
        void computeAllMoves();
        void computeAttackBoards();

        /* Move and Move Validation */
        void validateMovesForPiece(int position);
        bool validateMove(int fromIndex, int toIndex);
        bool movePiece(int fromIndex, int toIndex);
        bool existLegalMoves(Color color);
        bool isKingInCheck(Color color);

        /* En Passant Setters/Getters */
        int getEnPassantIndex() const {return enPassantIndex;}
        void setEnPassantIndex(int position) {enPassantIndex = position;}

};

#endif