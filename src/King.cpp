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
    mKingSideCastle = false;
    mQueenSideCastle = false;
    mInCheck = false;
}

/* Clones the piece to a new address */
Piece * King::clone(Board* newBoard) const {
    King * copy = new King(getColor(), getPosition(), newBoard, getHasMoved());
    copy->mKingSideCastle = mKingSideCastle;
    copy->mQueenSideCastle = mQueenSideCastle;
    copy->mInCheck = mInCheck;
    copy->validMoves = validMoves;

    return static_cast<Piece *>(copy);
}

/* ##### Static Variables ##### */
std::array<const int, 8> King::mOffsets = {-9, -8, -7, -1, +1, +7, +8, +9};

void King::computeMoves() {

    validMoves.clear();
    int fromIndex = getPosition();

    for (int offset : mOffsets) {
        int targetIndex = fromIndex + offset;
        int currentCol = fromIndex % 8;
        int nextCol = targetIndex % 8;
        if (!Board::isValidIndex(targetIndex)) continue;

        /* Prevent Wrap Around*/
        int colDiff = std::abs(nextCol - currentCol);
        
        SquareStatus status = mBoard->getSquareStatus(fromIndex, targetIndex);

        if ((status == SquareStatus::Empty || status == SquareStatus::Enemy) && colDiff <= 1) {
            if (getColor() == Color::White && !mBoard->blackAttackBoard[targetIndex]) {
                validMoves.push_back(targetIndex);
            }
                
            if (getColor() == Color::Black && !mBoard->whiteAttackBoard[targetIndex]) {
                validMoves.push_back(targetIndex);
            }
                
        }
        
    }

    if(!mHasMoved) computeCastling();
    else {
        mKingSideCastle = false;
        mQueenSideCastle = false;
    }
}

/* Computes the possibility of castle, according to the official chess rules. If the castle is possible, it will add it to the move list, otherwise not. If the piece already moved, it immediatelly returns false as the King has no rights to move */
void King::computeCastling() {
    int kingSquare = (getColor() == Color::White) ? 4 : 60;
    if (mHasMoved) return; /* If the king moved, then it is invalid*/
    if (mBoard->isKingInCheck(getColor())) return; /* If king is in check, you cannot castle*/

    /* Generate and Validate a King Side Castle */
    if (mKingSideCastle) {
        bool canCastle = true;
        Piece * rook = (getColor() == Color::White) ? mBoard->board[7] : mBoard->board[63];
        if (rook == nullptr) canCastle = false;
        if (rook && rook->getHasMoved()) {
            mKingSideCastle = false;
            canCastle = false;
        }

        /* Check if they are free */
        if (mBoard->board[kingSquare+1] || mBoard->board[kingSquare+2]) canCastle = false;

        /* Is the King in Check */
        if (mInCheck) canCastle = false;

        /* If the in between squares and final square are not attacked by enemy pieces */
        for (int i = 1; i <= 3; ++i) {
            int offset = kingSquare + i;
            if (mColor == Color::White) {
                if(mBoard->blackAttackBoard[offset]) canCastle = false;
            } else {
                if(mBoard->whiteAttackBoard[offset]) canCastle = false;
            }
        }

        if (canCastle)
            validMoves.push_back(kingSquare + 2);
    }

    if (mQueenSideCastle) {
        Piece * rook = (getColor() == Color::White) ? mBoard->board[0] : mBoard->board[56];
        bool canCastle = true;
        if (rook == nullptr) canCastle = false;
        if (rook && rook->getHasMoved()) {
            mQueenSideCastle = false;
            canCastle = false;
        }

        /* Check if they are free */
        if (mBoard->board[kingSquare-1] || mBoard->board[kingSquare-2] || mBoard->board[kingSquare-3]) canCastle = false;

        /* Is the King in Check */
        if (mInCheck) canCastle = false;

        /* If the in between squares and final square are not attacked by enemy pieces */
        for (int i = 1; i <= 3; ++i) {
            int offset = kingSquare - i;
            if (mColor == Color::White) {
                if(mBoard->blackAttackBoard[offset]) canCastle = false;
            } else {
                if(mBoard->whiteAttackBoard[offset]) canCastle = false;
            }
        }
        if (canCastle)
            validMoves.push_back(kingSquare - 2);
    }

}