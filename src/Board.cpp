/* Standard Libraries */
#include <cctype>
#include <iostream>
#include <sstream>
#include <cstdlib>

/* Include other defined headers */
#include "Board.hpp"
#include "Graphics.hpp"
#include "Game.hpp"

/* Include each piece type*/
#include "King.hpp"
#include "Queen.hpp"
#include "Rook.hpp"
#include "Bishop.hpp"
#include "Knight.hpp"
#include "Pawn.hpp"
#include "Piece.hpp"

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

/* Helper functions, maybe static attributes? */
int squareToIndex(int row, int col) {
    return row * COL + col;
}

int indexToRow(int index) {
    return index / ROW;
}

int indexToColumn(int index) {
    return index % COL;
}

Board::Board(ChessGame * game) : game(game), validMove(false), isChecked(false) {
    /* Set every pointer of board to NULL, ocupation board to 0 */
    for (int i = 0; i < ROW * COL; ++i) {
        board[i] = nullptr;
        whiteAttackBoard[i] = 0;
        blackAttackBoard[i] = 0;
    }
    whiteKing = nullptr;
    blackKing = nullptr;
}

Board::Board(const Board & original) {
     // Deep copy pieces
     for (int i = 0; i < 64; ++i) {
        board[i] = original.board[i] ? original.board[i]->clone(this) : nullptr;
        if (board[i] && board[i]->getType() == PieceType::King) {
            if (board[i]->getColor() == Color::White) whiteKing = static_cast<King *>(board[i]);
            else blackKing = static_cast<King *>(board[i]);
        }
    }
    // Copy attack boards
    whiteAttackBoard = original.whiteAttackBoard;
    blackAttackBoard = original.blackAttackBoard;
    
    game = nullptr;
}

Board::~Board() {
    clearBoard();
}

void Board::clearBoard() {
    for (int i = 0; i < ROW * COL; ++i) {
        delete board[i];
        board[i] = nullptr;
        whiteAttackBoard[i] = 0;
        blackAttackBoard[i] = 0;
        
    }
    whiteKing = nullptr;
    blackKing = nullptr;
}

Piece* Board::createPiece(PieceType type, Color color, int position) {
    switch(type) {
        case PieceType::King: return new King(color, position, this, false);
        case PieceType::Queen: return new Queen(color, position, this);
        case PieceType::Rook: return new Rook(color, position, this);
        case PieceType::Bishop: return new Bishop(color, position, this);
        case PieceType::Knight: return new Knight(color, position, this);
        case PieceType::Pawn: return new Pawn(color, position, this, false);
        default: return nullptr; // Implement error handling
    }
}

/* https://www.geeksforgeeks.org/how-to-split-string-by-delimiter-in-cpp/*/
/* https://en.wikipedia.org/wiki/Forsyth–Edwards_Notation */
bool Board::loadFromFEN(const std::string & fen) {
    
    clearBoard();
    
    std::istringstream fenStream(fen);
    std::string piecePlacement, activeColor, castlingRights;

    /* Split FEN into main parts */
    if (!(fenStream >> piecePlacement >> activeColor >> castlingRights)) {
        return false; /* Invalid FEN */
    }

    int row = (ROW - 1); /* Starts from the 8th row/rank */
    int col = 0; /* Starts from the 0th column/file */
    int index;

    /* Store the address of the White and Black King to setup the castling rights LATER */
    whiteKing = nullptr;
    blackKing = nullptr;

    for (char c : piecePlacement) {
        
        if(c == '/') {
            col = 0;
            row--;
        }
        
        else if (isdigit(c)) 
        {
            if((c - '0') > 0 && (c - '0') < 9) {
                int emptySquares = c - '0';
                col += emptySquares;
                if(col > (COL - 1)) col = COL - 1; /* Prevent overflow */
            }
        }

        else 
        {
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
            
            /* Two square moving rights for pawns */
            if ((row == 1 || row == 6) && type == PieceType::Pawn) {
                Pawn * pawn = static_cast<Pawn *>(board[index]);
                pawn->setHasMoved(false);
            }

            if (type == PieceType::King && color == Color::White) whiteKing = static_cast<King *>(board[index]);
            if (type == PieceType::King && color == Color::Black) blackKing = static_cast<King *>(board[index]);
            ++col;
            if (col > (COL - 1)) col = COL;
        }
    }

    /* Store turn information, change for a global variable later */
    Color turn = (activeColor == "w") ? Color::White :
    (activeColor == "b") ? Color::Black : Color::White;
    game->setTurn(turn);
    
    /* Castling Rights */
    if (whiteKing && blackKing) {
        if (castlingRights.find('K') != std::string::npos) {
            static_cast<King *>(whiteKing)->setKingSideCastleRight(true);
            if(board[7] != nullptr) board[7]->setHasMoved(false);
        }
        if(castlingRights.find('Q') != std::string::npos) {
            static_cast<King *>(whiteKing)->setQueenSideCastleRight(true);
            if(board[0] != nullptr) board[0]->setHasMoved(false);
        }
        if(castlingRights.find('k') != std::string::npos) {
            static_cast<King *>(blackKing)->setKingSideCastleRight(true);
            if(board[63] != nullptr) board[63]->setHasMoved(false);
        }
        if(castlingRights.find('q') != std::string::npos) {
            static_cast<King *>(blackKing)->setQueenSideCastleRight(true);
            if(board[56] != nullptr) board[56]->setHasMoved(false);
        }
    } else return false;

    /* Precompute valid moves */
    computeAllMoves();
    computeAttackBoards();    

    /* Recompute attack board for Kings */
    whiteKing->computeMoves();
    blackKing->computeMoves();

    return true;
}

SquareStatus Board::getSquareStatus(int fromIndex, int toIndex) const {
    
    if (board[fromIndex] == nullptr) return SquareStatus::Invalid;
    if (toIndex < 0 || toIndex > 63) return SquareStatus::Invalid;
    if (fromIndex < 0 || fromIndex > 63) return SquareStatus::Invalid;
    if(board[toIndex] == nullptr) return SquareStatus::Empty; /* empty */
    if(board[fromIndex]->getColor() != board[toIndex]->getColor()) return SquareStatus::Enemy; /* different piece color */
    if(board[fromIndex]->getColor() == board[toIndex]->getColor()) return SquareStatus::Friendly;

    return SquareStatus::Invalid;
}

void Board::computeAttackBoards() {
    
    for (int i = 0; i < 64; ++i) {
        whiteAttackBoard[i] = 0;
        blackAttackBoard[i] = 0;
    }

    for(int i = 0; i < 64; ++i) {
        if (board[i] != nullptr) {
            Color pieceColor = board[i]->getColor();
            for (int target : board[i]->validMoves) {
                if (pieceColor == Color::White) {
                    whiteAttackBoard[target] = 1;
                } else {
                    blackAttackBoard[target] = 1;
                }
            }
        }
    }
    
}

/* This method generates all moves, valid or invalid (pseudolegal moves) */
void Board::computeAllMoves() {
    // Clear all existing moves first
    system("clear");
    std::cerr << "Found piece, computing moves... ";
    for (int i = 0; i < 64; i++) {
        if (board[i] != nullptr) {
            board[i]->validMoves.clear();
        }
    }
    
    // Compute moves for each piece
    for (int i = 0; i < 64; i++) {
        std::cerr << "Checking square " << i << "... ";
        if (board[i] != nullptr) {
            std::cerr << "Found piece, computing moves... ";
            board[i]->computeMoves();
            std::cerr << "Done.\n";
        } else std::cerr << "Empty square.\n";
    }
    std::cerr << "Finished computeAllValidMoves()\n";
}

/* This method is responsible for VALIDATING the move */
void Board::validateMovesForPiece(int position) {
    if (position < 0 || position > 63) return;
    Piece * piece = board[position];
    if (!piece) return;

    // Get all pseudolegal moves first
    std::vector<int> pseudolegalMoves;
    piece->computeMoves();  // Generates pseudolegal moves
    pseudolegalMoves = piece->validMoves;
    std::vector<int> legalMoves;
    
    /* Validate each move */
    for (int target : pseudolegalMoves) {
        /* Create a temporary board copy */
        if(validateMove(position, target)) {
            legalMoves.push_back(target);
        }
    }

    piece->validMoves = legalMoves;
}

bool Board::validateMove(int fromIndex, int toIndex) {
    /* Checks if the indexes are within the bounds */
    validMove = false;
    if (fromIndex < 0 || fromIndex >= 64 || toIndex < 0 || toIndex >= 64)
        return false;
    Piece* movingPiece = board[fromIndex];
    if (!movingPiece) return false;

    /* Prevent KING CAPTURE */
    Piece * targetPiece = board[toIndex];
    if (targetPiece && targetPiece->getType() == PieceType::King)
        return false;

    // 3. Create temporary board for validation
    Board tempBoard(*this);
    
    // 4. Simulate move on temporary board
    if (tempBoard.board[toIndex] && tempBoard.board[toIndex]->getType() != PieceType::King) delete tempBoard.board[toIndex];
    tempBoard.board[toIndex] = tempBoard.board[fromIndex];
    tempBoard.board[fromIndex] = nullptr;
    tempBoard.board[toIndex]->setPosition(toIndex);
    
    
    // 5. Update king pointers in temp board if needed
    if (movingPiece->getType() == PieceType::King) {
        if (movingPiece->getColor() == Color::White) {
            tempBoard.whiteKing = static_cast<King*>(tempBoard.board[toIndex]);
        } else {
            tempBoard.blackKing = static_cast<King*>(tempBoard.board[toIndex]);
        }
    }
    

    // 6. Validate move doesn't leave king in check
    tempBoard.computeAllMoves();
    tempBoard.computeAttackBoards();
    
    if (tempBoard.isKingInCheck(movingPiece->getColor())) {
        validMove = false;
        return false; // Move would leave king in check
    } else {
        validMove = true;
        return true;
    }
}

bool Board::movePiece(int fromIndex, int toIndex) {
    
    if(!validMove)
        return false;

    /* Checks if the indexes are within the bounds */
    if (fromIndex < 0 || fromIndex >= 64 || toIndex < 0 || toIndex >= 64)
        return false;
    Piece* movingPiece = board[fromIndex];
    if (!movingPiece) return false;

    /* Prevent KING CAPTURE */
    Piece * targetPiece = board[toIndex];
    if (targetPiece && targetPiece->getType() == PieceType::King)
        return false;

    // 7. Execute real move (passed validation)
    if (targetPiece && targetPiece->getType() != PieceType::King) delete targetPiece;
    board[fromIndex] = nullptr;
    board[toIndex] = movingPiece;
    movingPiece->setPosition(toIndex);
    movingPiece->setHasMoved(true);

    // 8. Update king pointers if needed
    if (movingPiece->getType() == PieceType::King) {
        if (movingPiece->getColor() == Color::White) {
            whiteKing = static_cast<King*>(movingPiece);
            
        } else {
            blackKing = static_cast<King*>(movingPiece);
            
        }
    }

    // 9. Update game state
    computeAllMoves();
    computeAttackBoards();
    whiteKing->computeMoves();
    blackKing->computeMoves();

    /* Reset validMove flag */
    validMove = false;

    /* Reset check status after a move... */
     // 9. Update game state
    computeAllMoves();
    computeAttackBoards();
    whiteKing->computeMoves();
    blackKing->computeMoves();

    /* Reset validMove flag */
    validMove = false;

    /* Reset check status after a move... */
    if (movingPiece->getType() == PieceType::King) {
        whiteKing->setCheck(false);
        blackKing->setCheck(false);
    } else {
        bool whiteInCheck = isKingInCheck(Color::White);
        bool blackInCheck = isKingInCheck(Color::Black);
        whiteKing->setCheck(whiteInCheck);
        blackKing->setCheck(blackInCheck);
    }
    

    return true;

}

bool Board::isKingInCheck(Color color) {
    King * king = (color == Color::White) ? static_cast<King *>(whiteKing) : static_cast<King *>(blackKing);
    if (king == nullptr) return false;
    king->setCheck(false);
    isChecked = false;

    int kingPos = king->getPosition();
    bool isAttacked;
    if (color == Color::White) {
        isAttacked = (blackAttackBoard[kingPos] != 0);
        if (isAttacked) king->setCheck(true);
        else king->setCheck(false);
    } else {
        isAttacked = (whiteAttackBoard[kingPos] != 0);
        if (isAttacked) king->setCheck(true);
        else king->setCheck(false);
    }
    if (isAttacked) isChecked = true;
    return isAttacked;
}