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
std::array<const int, 8> Knight::offsets = {-17, -15, -10, -6, 6, 10, 15, 17};

void Knight::computeValidMoves() {
    validMoves.clear();
    int fromIndex = getPosition();
    int fromRow = fromIndex / 8;
    int fromCol = fromIndex % 8;

    for (int offset : offsets) {
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
        
        SquareStatus status = board->getSquareStatus(fromIndex, targetIndex);
        if (status == SquareStatus::Empty || status == SquareStatus::Enemy) {
            validMoves.push_back(targetIndex);
            if (board->board[targetIndex]->getType() == PieceType::King && board->board[targetIndex]->getColor() != getColor()) {
                King * king = static_cast<King *>(board->board[targetIndex]);
                king->setCheck(true);
            }
        }
    }
}