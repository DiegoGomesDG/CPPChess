#include "Board.hpp"
#include "Piece.hpp"
#include "King.hpp"

/* https://www.chessprogramming.org/Efficient_Generation_of_Sliding_Piece_Attacks
    +7    +8    +9
        \  |  /
    -1 <-  0 -> +1
        /  |  \
    -9    -8    -7
*/

/* Constructor */
King::King(Color color, int position, Board * board, bool hasMoved) : Piece(color, PieceType::King, position, board, hasMoved) {
    kingSideCastle = false;
    queenSideCastle = false;
    inCheck = false;
}

/* Clones the piece to a new address */
Piece * King::clone(Board* newBoard) const {
    King * copy = new King(getColor(), getPosition(), newBoard, getHasMoved());
    copy->kingSideCastle = kingSideCastle;
    copy->queenSideCastle = queenSideCastle;
    copy->inCheck = inCheck;
    copy->validMoves = validMoves;

    return static_cast<Piece *>(copy);
}

/* ##### Static Variables ##### */
std::array<const int, 8> King::offsets = {-9, -8, -7, -1, +1, +7, +8, +9};

void King::computeMoves() {
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

    if(!hasMoved) computeCastling();
    else {
        kingSideCastle = false;
        queenSideCastle = false;
    }
}

/* Computes the possibility of castle, according to the official chess rules. If the castle is possible, it will add it to the move list, otherwise not. If the piece already moved, it immediatelly returns false as the King has no rights to move */
void King::computeCastling() {
    int kingSquare = (getColor() == Color::White) ? 4 : 60;
    if (hasMoved) {
        return;
    } /* If the king moved, then it is invalid*/

    /* Generate and Validate a King Side Castle */
    if (kingSideCastle) {
        bool canCastle = true;
        Piece * rook = (getColor() == Color::White) ? board->board[7] : board->board[63];
        if (rook == nullptr) canCastle = false;
        if (rook && rook->getHasMoved()) {
            kingSideCastle = false;
            canCastle = false;
        }

        /* Check if they are free */
        if (board->board[kingSquare+1] || board->board[kingSquare+2]) canCastle = false;

        /* Is the King in Check */
        if (inCheck) canCastle = false;

        /* If the in between squares and final square are not attacked by enemy pieces */
        for (int i = 1; i <= 3; ++i) {
            int offset = kingSquare + i;
            if (color == Color::White) {
                if(board->blackAttackBoard[offset]) canCastle = false;
            } else {
                if(board->whiteAttackBoard[offset]) canCastle = false;
            }
        }

        if (canCastle)
            validMoves.push_back(kingSquare + 2);

    }

    if (queenSideCastle) {
        Piece * rook = (getColor() == Color::White) ? board->board[0] : board->board[56];
        bool canCastle = true;
        if (rook == nullptr) canCastle = false;
        if (rook && rook->getHasMoved()) {
            queenSideCastle = false;
            canCastle = false;
        }

        /* Check if they are free */
        if (board->board[kingSquare-1] || board->board[kingSquare-2] || board->board[kingSquare-3]) canCastle = false;

        /* Is the King in Check */
        if (inCheck) canCastle = false;

        /* If the in between squares and final square are not attacked by enemy pieces */
        for (int i = 1; i <= 3; ++i) {
            int offset = kingSquare - i;
            if (color == Color::White) {
                if(board->blackAttackBoard[offset]) canCastle = false;
            } else {
                if(board->whiteAttackBoard[offset]) canCastle = false;
            }
        }
        if (canCastle)
            validMoves.push_back(kingSquare - 2);
    }

}