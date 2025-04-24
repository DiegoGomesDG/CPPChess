/* Standard Libraries */
#include <cctype>
#include <iostream>
#include <sstream>

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

Board::Board(ChessGame * game) : game(game) {
    /* Set every pointer of board to NULL, ocupation board to 0 */
    for (int i = 0; i < ROW * COL; ++i) {
        board[i] = nullptr;
        whiteAttackBoard[i] = 0;
        blackAttackBoard[i] = 0;
    }
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
}

Piece* Board::createPiece(PieceType type, Color color, int position) {
    switch(type) {
        case PieceType::King: return new King(color, position, this);
        case PieceType::Queen: return new Queen(color, position, this);
        case PieceType::Rook: return new Rook(color, position, this);
        case PieceType::Bishop: return new Bishop(color, position, this);
        case PieceType::Knight: return new Knight(color, position, this);
        case PieceType::Pawn: return new Pawn(color, position, this);
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
    King * whiteKing = nullptr;
    King * blackKing = nullptr;

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
                pawn->setDoublePush(true);
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
        whiteKing->setKingSideCastleRight(castlingRights.find('K') != std::string::npos);
        whiteKing->setQueenSideCastleRight(castlingRights.find('Q') != std::string::npos);
        blackKing->setKingSideCastleRight(castlingRights.find('k') != std::string::npos);
        blackKing->setQueenSideCastleRight(castlingRights.find('q') != std::string::npos);
    }

    std::cout << std::endl << static_cast<int>(turn) << std::endl;

    /* Precompute valid moves */
    for (Piece * piece : board) {
        if (piece != nullptr)
            piece->computeValidMoves();
    }    

    /* Compute attack boards*/
    computeAttackBoards();
    printAttackBoards();

    /* Recompute attack board for Kings */
    whiteKing->computeValidMoves();
    blackKing->computeValidMoves();

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

    for (Piece * piece : board) {
    if (piece != nullptr)
        for (int index : piece->validMoves) {
            switch (piece->getColor()) {
                case Color::White: whiteAttackBoard[index] = 1; break;
                case Color::Black: blackAttackBoard[index] = 1; break;
                default: break;
            }
                    
        }

    }    
}

void Board::printAttackBoards() {
   
    std::cout << std::endl << std::endl << std::endl;
    int row = 7, col = 0;
    int index;
    
    while (!(row == 0 && col == 8)) {
        index = squareToIndex(row, col);
        if (whiteAttackBoard[index])
            std::cout << "* ";
        else std::cout << "_ ";
    
        ++col;
        if(col == 8 && row != 0) {
            std::cout << std::endl;
            col = 0;
            if (row >= 0) row--;
        }
    }
        std::cout << std::endl << "----------------" << std::endl;

    row = 7; col = 0;
    while (!(row == 0 && col == 8)) {
        index = squareToIndex(row, col);
        if (blackAttackBoard[index])
            std::cout << "* ";
        else std::cout << "_ ";
    
        ++col;
        if(col == 8 && row != 0) {
            std::cout << std::endl;
            col = 0;
            if (row >= 0) row--;
        }
    }
        std::cout << std::endl;
}

void Board::movePiece(int fromIndex, int toIndex) {
    
    /* Retrieve the Moving Piece, check if it is not null */
    Piece * movingPiece = board[fromIndex];
    if (movingPiece == nullptr) return;

    //movingPiece->computeValidMoves();

    /* Check if the move is valid */
    bool isMoveValid = movingPiece->isValidMove(toIndex);
    if (!isMoveValid) return;

    /* Handle PIECE CAPTURE. If the piece at toIndex is not null, it means there is some piece, which should be deleted */
    Piece * targetPiece = board[toIndex];
    if (targetPiece != nullptr && targetPiece->getType() != PieceType::King)
        delete targetPiece;

    /* Move Piece */
    board[toIndex] = movingPiece;
    board[fromIndex] = nullptr;
    movingPiece->setPosition(toIndex);

    /* TEMPORARY: Removes the ability to have double forward moves. There should be a better way to do it... */
    if(movingPiece->getType() == PieceType::Pawn) {
        Pawn * pawn = static_cast<Pawn *>(movingPiece);
        pawn->setDoublePush(false);
    }

    /* Recompute the valid moves */
    Piece * whiteKing = nullptr;
    Piece * blackKing = nullptr;
    for (Piece * piece : board) {
        if (piece != nullptr) {
            piece->computeValidMoves();
            if (piece->getType() == PieceType::King) {
                if(piece->getColor() == Color::White) whiteKing = piece;
                if(piece->getColor() == Color::Black) blackKing = piece;
            }
        }
            
        
    }
    /* Recompute attack boards */
    computeAttackBoards();
    whiteKing->computeValidMoves();
    blackKing->computeValidMoves();
}