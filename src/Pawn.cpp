#include "Board.hpp"
#include "Pawn.hpp"

void Pawn::computeValidMoves(const Board& board) {
    validMoves.clear();
    int fromIndex = getPosition();
    Color color = getColor();

    int forward = (color == Color::White) ? 8 : -8;
    int doubleForward = (color == Color::White) ? 16 : -16;
    int captureLeft = (color == Color::White) ? 7 : -9;
    int captureRight = (color == Color::White) ? 9 : -7;

    int currentCol = fromIndex % 8;

    /* Single move forward */
    int targetIndex = fromIndex + forward;
    SquareStatus status = board.getSquareStatus(fromIndex, targetIndex);
    if (status == SquareStatus::Empty && (targetIndex >= 0 && targetIndex < 64)) {
        forwardMoves.push_back(targetIndex);

        /* Double move forward */
        targetIndex = fromIndex + doubleForward;
        status = board.getSquareStatus(fromIndex, targetIndex);
        if (status == SquareStatus::Empty && (targetIndex >= 0 && targetIndex < 64) && doublePush) {
            forwardMoves.push_back(targetIndex);
        }
    }

    /* Capture to Left */
    targetIndex = fromIndex + captureLeft;
    int nextCol = targetIndex % 8;
    int colDiff = std::abs(nextCol - currentCol);

    status = board.getSquareStatus(fromIndex, targetIndex);
    if ((status == SquareStatus::Enemy || status == SquareStatus::Empty) && colDiff == 1) {
        validMoves.push_back(targetIndex);
    }

    /* Capture to Right */
    targetIndex = fromIndex + captureRight;
    nextCol = targetIndex % 8;
    colDiff = std::abs(nextCol - currentCol);

    status = board.getSquareStatus(fromIndex, targetIndex);
    if ((status == SquareStatus::Enemy || status == SquareStatus::Empty) && colDiff == 1) {
        validMoves.push_back(targetIndex);
    }

}