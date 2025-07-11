#include "Board.hpp"
#include "Pawn.hpp"
#include "King.hpp"

/* Clones the piece to a new index */
Piece * Pawn::clone(Board* newBoard) const {
    Piece * copy = new Pawn(getColor(), getPosition(), newBoard, getHasMoved());
    copy->validMoves = validMoves;
    return copy;
}

/* Computes all pseudomoves for the Pawn, with the offsets being generated according to the color. Pawn is the only move that can move or capture in one direction, meaning it cannot go back or capture backwards. Also, it takes into account the possibility of a valid en-passant square */
void Pawn::computeMoves() {
    validMoves.clear();
    //forwardMoves.clear();
    int fromIndex = getPosition();
    Color color = getColor();

    int forward = (color == Color::White) ? 8 : -8;
    int doubleForward = (color == Color::White) ? 16 : -16;
    int captureLeft = (color == Color::White) ? 7 : -9;
    int captureRight = (color == Color::White) ? 9 : -7;

    int currentCol = fromIndex % 8;

    /* Single move forward */
    int targetIndex = fromIndex + forward;
    SquareStatus status = mBoard->getSquareStatus(fromIndex, targetIndex);
    if (status == SquareStatus::Empty && Board::isValidIndex(targetIndex)) {
        validMoves.push_back(targetIndex);

        /* Double move forward */
        targetIndex = fromIndex + doubleForward;
        status = mBoard->getSquareStatus(fromIndex, targetIndex);
        if (status == SquareStatus::Empty && (Board::isValidIndex(targetIndex)) && !getHasMoved()) {
            validMoves.push_back(targetIndex);
        }
    }

    /* Capture to Left */
    targetIndex = fromIndex + captureLeft;
    int nextCol = targetIndex % 8;
    int colDiff = std::abs(nextCol - currentCol);

    status = mBoard->getSquareStatus(fromIndex, targetIndex);
    if ((status == SquareStatus::Enemy) && colDiff == 1) {
        validMoves.push_back(targetIndex);
    }
    if ((status == SquareStatus::Empty) && colDiff == 1 && targetIndex == mBoard->getEnPassantIndex()) {
        validMoves.push_back(targetIndex);
    }

    /* Capture to Right */
    targetIndex = fromIndex + captureRight;
    nextCol = targetIndex % 8;
    colDiff = std::abs(nextCol - currentCol);

    status = mBoard->getSquareStatus(fromIndex, targetIndex);
    if ((status == SquareStatus::Enemy) && colDiff == 1) {
        validMoves.push_back(targetIndex);
    }
    if ((status == SquareStatus::Empty) && colDiff == 1 && targetIndex == mBoard->getEnPassantIndex()) {
        validMoves.push_back(targetIndex);
    }

}