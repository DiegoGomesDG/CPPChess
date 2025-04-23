#include "Board.hpp"
#include "King.hpp"

/* https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
    +7    +8    +9
        \  |  /
    -1 <-  0 -> +1
        /  |  \
    -9    -8    -7
*/

King::King(Color color, int position, Board * board) : Piece(color, PieceType::King, position, board) {
    kingSideCastle = false;
    queenSideCastle = false;
    inCheck = false;
    hasMoved = false;
}

/* ##### Static Variables ##### */
std::array<const int, 8> King::offsets = {-9, -8, -7, -1, +1, +7, +8, +9};

void King::computeValidMoves() {
    validMoves.clear();
    int fromIndex = getPosition();

    for (int offset : offsets) {
        int targetIndex = fromIndex + offset;
        if (targetIndex < 0 || targetIndex >= 64) continue;
        
        SquareStatus status = board->getSquareStatus(fromIndex, targetIndex);

        if (status == SquareStatus::Empty || status == SquareStatus::Enemy) {
            if (getColor() == Color::White && !board->blackAttackBoard[targetIndex])
                validMoves.push_back(targetIndex);
            if (getColor() == Color::Black && !board->whiteAttackBoard[targetIndex])
                validMoves.push_back(targetIndex);
        }
        
    }
}

