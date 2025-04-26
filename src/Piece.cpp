#include "Board.hpp"
#include "Piece.hpp"

/* Loops through each move using a Linear Search and determines if the parameter's destination index matches with any of the moves in the vector */
bool Piece::isValidMove(int toIndex) {
    bool isMoveValid = false;
    for (int move : validMoves) {
        if (move == toIndex) {
            isMoveValid = true;
            break;
        }
    }
    return isMoveValid;
}