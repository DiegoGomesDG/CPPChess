#include "Board.hpp"
#include "Rook.hpp"
#include "King.hpp"
#include <array>

/* https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
    +7    +8    +9
        \  |  /
    -1 <-  0 -> +1
        /  |  \
    -9    -8    -7
*/

/* ##### Static Variables ##### */
std::array<const int, 4> Rook::mOffsets = {-8, -1, +1, +8};

/* Clone method to allow the piece to be copied into a new memory location. Useful for creating a copy of Board */
Piece * Rook::clone(Board* newBoard) const {
    Piece * copy = new Rook(getColor(), getPosition(), newBoard);
    copy->validMoves = validMoves;
    return copy;
}

/* Compute all possible pseudomoves according to the piece offset */
void Rook::computeMoves() {
    validMoves.clear();
    int fromIndex = getPosition();
    std::vector<int> pseudoMoves;

    for (int offset : mOffsets) {
        int targetIndex = fromIndex;
        while(true) {
            int currentCol = targetIndex % 8;
            targetIndex += offset;
            if (targetIndex < 0 || targetIndex >= 64) break;
            
            /* Prevent wrap around */
            int nextCol = targetIndex % 8;

            if ((offset == -1 || offset == 1) && std::abs(nextCol - currentCol) != 1) break;

            /* Compares the origin square to the destination square and determines if it is Empty, Invalid, Friendly or Enemy */
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