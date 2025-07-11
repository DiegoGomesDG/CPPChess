#include "Board.hpp"
#include "Knight.hpp"
#include "King.hpp"

/* https://www.chessprogramming.org/Knight_Pattern
            +15  +17
        +6 __|     |__+10
              \   /
               >0<
           __ /   \ __
       -10   |     |   -6
            -17  -15
*/

/* ##### Static Variables ##### */
std::array<const int, 8> Knight::mOffsets = {-17, -15, -10, -6, 6, 10, 15, 17};

Piece * Knight::clone(Board* newBoard) const {
    Piece * copy = new Knight(mColor, mPosition, newBoard);
    copy->validMoves = validMoves;
    return copy;
}

/* Compute all possible pseudomoves based on the Knight's offsets */
void Knight::computeMoves() {
    validMoves.clear();
    
    int fromIndex = getPosition();
    int fromRow = fromIndex / 8;
    int fromCol = fromIndex % 8;

    for (int offset : mOffsets) {
        int targetIndex = fromIndex + offset;
        if (targetIndex < 0 || targetIndex >= 64)
            continue;

        int toRow = targetIndex / 8;
        int toCol = targetIndex % 8;

        int rowDiff = std::abs(toRow - fromRow);
        int colDiff = std::abs(toCol - fromCol);

        // Valid knight move should have (rowDiff, colDiff) == (2, 1) or (1, 2)
        if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2)))
            continue;  // Invalid — wrapped around
        
        SquareStatus status = mBoard->getSquareStatus(fromIndex, targetIndex);
        if (status == SquareStatus::Empty || status == SquareStatus::Enemy) {
            validMoves.push_back(targetIndex);
        }
    }

}