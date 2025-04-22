#include "Board.hpp"
#include "Knight.hpp"

/* https://www.chessprogramming.org/Knight_Pattern
            +15  +17
             |     |
        +6 __|     |__+10
              \   /
               >0<
           __ /   \ __
       -10   |     |   -6
             |     |
            -17  -15
*/

/* ##### Static Variables ##### */
std::array<const int, 8> Knight::offsets = {-17, -15, -10, -6, 6, 10, 15, 17};

void Knight::possibleMoves(const Board& board) {
    validMoves.clear();
    int fromIndex = getPosition();
    
    for (int offset : offsets) {
        int targetIndex = fromIndex + offset;
        SquareStatus status = board.getSquareStatus(fromIndex, targetIndex);
        if (status == SquareStatus::Empty || status == SquareStatus::Enemy) {
            validMoves.push_back(targetIndex);
        }
    }
}