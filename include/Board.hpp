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
public:
    Board(ChessGame * game); /* Constructor */
    Board(const Board & original); /* Copy Constructor */
    ~Board(); /* Destructor */

    std::array<int, 64> whiteAttackBoard;
    std::array<int, 64> blackAttackBoard;
    std::array<Piece *, 64> board; /* PUBLIC for simplifying purposes*/
    int castlingOffset; /* just for the animation */
    int moveCount;

    /* Static Methods - Convertion of (row, col) to index and back */
    static int squareToIndex(int row, int col);
    static int indexToRow(int index);
    static int indexToColumn(int index);

    /* Static Methods - Conversion of an index to Algebraic Notation (e.g. e4, f2, etc) and the otherway `*/
    static int algebraicToIndex(const std::string & notation);
    static std::string indexToAlgebraic(int index);

    /* Get the King */
    King * getKing(Color color) { return (color == Color::White) ? mWhiteKing : mBlackKing; }

    /* Clear/Reset the board*/
    void clearBoard();

    /* Piece creation and board initialization */
    Piece * createPiece(PieceType type, Color color, int position);
    bool loadFromFEN(const std::string & fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    /* Helpers for move computation */
    static bool isValidIndex(int index);
    SquareStatus getSquareStatus(int fromIndex, int toIndex) const;

    /* Compute moves and attack boards*/
    void computeAllMoves();
    void computeAttackBoards();

    /* Move, Move Validation, Checkmate detection */
    void validateMovesForPiece(int position);
    bool validateMove(int fromIndex, int toIndex);
    void validateAllNextPlayerMoves(Color turn);

    bool movePiece(int fromIndex, int toIndex);
    bool existLegalMoves(Color color);
    bool isKingInCheck(Color color);

    void countMoves(Color color);

    /* En Passant Setters/Getters */
    int getEnPassantIndex() const { return mEnPassantIndex; }
    void setEnPassantIndex(int position) { mEnPassantIndex = position; }

private:
    ChessGame * mGamePtr;
    King * mWhiteKing;
    King * mBlackKing;
    int mEnPassantIndex;
};

#endif