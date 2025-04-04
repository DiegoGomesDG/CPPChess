/* Standard Libraries */
#include <cctype>

/* Include other defined headers */
#include "board.hpp"


/* Include each piece type*/
#include "king.hpp"
#include "queen.hpp"
#include "rook.hpp"
#include "bishop.hpp"
#include "knight.hpp"
#include "pawn.hpp"
#include "piece.hpp"


/* 1D Array Board Indexes
   |00|01|02|03|04|05|06|07
07| 56 57 58 59 60 61 62 63 
06| 48 49 50 51 52 53 54 55
05| 40 41 42 43 44 45 46 47
04| 32 33 34 35 36 37 38 39
03| 24 25 26 27 28 29 30 31
02| 16 17 18 19 20 21 22 23
01| 08 09 10 11 12 13 14 15
00| 00 01 02 03 04 05 06 07

Source:
https://lichess.org/@/likeawizard/blog/review-of-different-board-representations-in-computer-chess/S9eQCAWa
*/

int squareToIndex(int row, int col) {
    return row * 8 + col;
}

int indexToRow(int index) {
    return index / 8;
}

int indexToColumn(int index) {
    return index % 8;
}

Board::Board() {
    
    /* Set every pointer of board to NULL, ocupation board to 0 */
    for (int i = 0; i < 64; ++i) {
        board[i] = NULL;
        whiteOccupation[i] = 0;
        blackOccupation[i] = 0;
    }

}

Board::~Board() {
    clearBoard();
}

void Board::clearBoard() {
    for (int i = 0; i < 64; ++i) {
        delete board[i];
        board[i] = NULL;
        whiteOccupation[i] = 0;
        blackOccupation[i] = 0;
    }
}

Piece* Board::createPiece(PieceType type, Color color, int position) {
    switch(type) {
        case PieceType::King: return new King(color, position);
        case PieceType::Queen: return new Queen(color, position);
        case PieceType::Rook: return new Rook(color, position);
        case PieceType::Bishop: return new Bishop(color, position);
        case PieceType::Knight: return new Knight(color, position);
        case PieceType::Pawn: return new Pawn(color, position);
        default: return nullptr; // Implement error handling
    }
}

/* https://en.wikipedia.org/wiki/Forsyth–Edwards_Notation */
bool Board::loadFromFEN(const std::string & fen) {
    clearBoard();
    int row = 7; /* Starts from the 8th row/rank */
    int col = 0; /* Starts from the 0th column/file */

    int strLen = fen.length();
    char c;
    int index;
    /* Some improvement will be needed if is desired to support full FERN, for now, only the 
    Piece Placement is supported, any extra character might result in misbehavior*/
    for(int i = 0; i < strLen; ++i) {
        c = fen[i];
        if(c == '/') {
            col = 0;
            row--;
        }

        else if (isdigit(c)) {
            if((c - '0') > 0 && (c - '0') < 9) {
                int emptySquares = c - '0';
                col += emptySquares;
                if(col > 7) col = 7; /* Prevent overflow */
            }
        }

        else {
            Color color = (isupper(c)) ? Color::White : Color::Black;
            PieceType type;
            c = tolower(c);
            switch(c) {    
                case 'k': type = PieceType::King; break;
                case 'q': type = PieceType::Queen; break;
                case 'r': type = PieceType::Rook; break;
                case 'n': type = PieceType::Knight; break;
                case 'b': type = PieceType::Bishop; break;
                case 'p': type = PieceType::Pawn; break;
                default: return false; /* Invalid Piece */
            }
            index = squareToIndex(row, col);
            board[index] = createPiece(type, color, index);
            ++col;
            if (col > 7) col = 7;
        }
        
    }

    return true;
}