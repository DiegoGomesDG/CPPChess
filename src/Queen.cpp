#include "Board.hpp"
#include "Queen.hpp"

/* https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
    +7    +8    +9
        \  |  /
    -1 <-  0 -> +1
        /  |  \
    -9    -8    -7
*/

/* ##### Static Variables ##### */
std::array<const int, 8> Queen::offsets = {-9, -8, -7, -1, +1, +7, +8, +9};

void Queen::computeValidMoves() {
    validMoves.clear();
    int fromIndex = getPosition();

    for (int offset : offsets) {
        int targetIndex = fromIndex;
        while(true) {
            int currentCol = targetIndex % 8;
            targetIndex += offset;
            if (targetIndex < 0 || targetIndex >= 64) break;
            
            /* Prevent wrap around */
            int nextCol = targetIndex % 8;

            int colDiff = std::abs(nextCol - currentCol);
            if (colDiff != 1 && colDiff != 0) break;

            SquareStatus status = board->getSquareStatus(fromIndex, targetIndex);

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