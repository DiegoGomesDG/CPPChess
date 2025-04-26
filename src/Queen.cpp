#include "Board.hpp"
#include "Queen.hpp"
#include "King.hpp"
#include "Piece.hpp"

/* https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
    +7    +8    +9
        \  |  /
    -1 <-  0 -> +1
        /  |  \
    -9    -8    -7
*/

/* ##### Static Variables ##### */
std::array<const int, 8> Queen::offsets = {-9, -8, -7, -1, +1, +7, +8, +9};

/* Clone method to allow the piece to be copied into a new memory location. Useful for creating a copy of Board */
Piece * Queen::clone(Board* newBoard) const {
    Piece * copy = new Queen(getColor(), getPosition(), newBoard);
    copy->validMoves = validMoves;
    return copy;
}

/* Compute all possible pseudomoves according to the piece offsets */
void Queen::computeMoves() {
    validMoves.clear();
    int fromIndex = getPosition();
    std::vector<int> pseudoMoves;

    for (int offset : offsets) {
        int targetIndex = fromIndex;
        while(true) {
            int currentCol = targetIndex % 8;
            targetIndex += offset;
            if (targetIndex < 0 || targetIndex >= 64) break;
            
            /* Prevent wrap around */
            int nextCol = targetIndex % 8;

            if ((offset == -1 || offset == 1 || offset == -9 || offset == -7 || offset == 7 || offset == 9) &&
                std::abs(nextCol - currentCol) > 1) break;

            
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