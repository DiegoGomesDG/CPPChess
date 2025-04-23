#include "Board.hpp"
#include "Piece.hpp"

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