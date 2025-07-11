#include "Board.hpp"
#include "King.hpp"
#include "Piece.hpp"
#include "Bishop.hpp"
#include <iostream>

/* https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
    +7    +8    +9
        \  |  /
    -1 <-  0 -> +1
        /  |  \
    -9    -8    -7
*/

/* ##### Static Variables ##### */
std::array<const int, 4> Bishop::mOffsets = {-9, -7, +7, +9};

/* Clone method to allow the piece to be copied into a new memory location. Useful for creating a copy of Board */
Piece * Bishop::clone(Board* newBoard) const {
    Piece * copy = new Bishop(getColor(), getPosition(), newBoard);
    copy->validMoves = validMoves;
    return copy;
}

/* Compute all possible pseudomoves according to the piece offsets */
void Bishop::computeMoves() {
    validMoves.clear();
    int fromIndex = getPosition();

    /* Generate all pseudolegal moves (without considering checks, pins, etc) */
    for (int offset : mOffsets) {
        int targetIndex = fromIndex;
        while(true) {
            int currentCol = targetIndex % 8;
            targetIndex += offset;
            if (targetIndex < 0 || targetIndex >= 64) break;
            
            /* Prevent wrap around */
            int nextCol = targetIndex % 8;

            int colDiff = std::abs(nextCol - currentCol);
            if (colDiff != 1 && colDiff != 0) break;

            
            SquareStatus status = mBoard->getSquareStatus(fromIndex, targetIndex);
            if (status == SquareStatus::Empty) {
                validMoves.push_back(targetIndex);
            } else if (status == SquareStatus::Enemy) {   
                validMoves.push_back(targetIndex);
                break;
            } else if (status == SquareStatus::Friendly) {
                break;
            }
        }
    }

}