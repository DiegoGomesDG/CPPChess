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

/* ##### Static Methods - Helper functions ##### */
/* Converts row and column of a square in the board to an index */
int Board::squareToIndex(int row, int col) {
    return row * COL + col;
}

/* Returns the row in which the index represents */
int Board::indexToRow(int index) {
    return index / ROW;
}

/* Returns the column that the index represents */
int Board::indexToColumn(int index) {
    return index % COL;
}

/* Converts Chess Algebraic notation to an Index */
int Board::algebraicToIndex(const std::string & notation) {
    /* A pure (without any marking of checks or captures) algebraic notation should have two characters, one containing the file and the rank */
    if (notation.length() != 2)
        throw std::invalid_argument("Invalid algebraic notation length");

    int col = -1, row = -1;
    char file = notation[0];
    if (file >= 'a' && file <= 'h') {
        col = file - 'a';
    } 
    else throw std::invalid_argument("Invalid file character in algebraic notation");

    char rank = notation[1];
    if (file >= '1' && file <= '8') {
        row = rank - '1';
    } 
    else std::invalid_argument("Invalid rank character in algebraic notation");

    return Board::squareToIndex(row, col);
}

/* Converts an index of the board to a chess Algebraic Notation */
std::string Board::indexToAlgebraic(int index) {
    if(!isValidIndex(index)) {
        std::string empty = "";
        return empty;
    }

    char file = 'a' + indexToColumn(index);
    char rank = '1' + indexToRow(index);

    return std::string() + file + rank;
}

/* Constructor */
Board::Board(ChessGame * game) : game(game), enPassantIndex(-1) {
    /* Set every pointer of board to NULL, ocupation board to 0 */
    for (int i = 0; i < ROW * COL; ++i) {
        board[i] = nullptr;
        whiteAttackBoard[i] = 0;
        blackAttackBoard[i] = 0;
    }
    whiteKing = nullptr;
    blackKing = nullptr;
}

/* Copy Constructor, which will copy every piece of the Board with new addresses, so move simulations can be performed */
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

    /* Copying enPassant */
    enPassantIndex = original.getEnPassantIndex();

    /* Game Pointer is not necessary */
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
    enPassantIndex = -1;
}

/* Dynamically allocates a piece according to the type, color and position */
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

/* Initializes a Chess Position by providing a VALID FEN String, which has the information of the pieces placement, turn, castling rights, en-passant target square, and other informations 
More information: https://en.wikipedia.org/wiki/Forsyth–Edwards_Notation */
bool Board::loadFromFEN(const std::string & fen) {
    
    clearBoard();
    /* https://www.geeksforgeeks.org/how-to-split-string-by-delimiter-in-cpp/*/
    std::istringstream fenStream(fen);
    std::string piecePlacement, activeColor, castlingRights, enPassant, halfMoveClock, fullMoveClock;

    /* Split FEN into main parts */
    if (!(fenStream >> piecePlacement >> activeColor >> castlingRights >> enPassant >> halfMoveClock >> fullMoveClock)) {
        throw std::invalid_argument("FEN string is incomplete or malformed");
    }

    int row = (ROW - 1); /* Starts from the 8th row/rank */
    int col = 0; /* Starts from the 0th column/file */
    int index;

    /* Initialize pointers to the white and black kings */
    whiteKing = nullptr;
    blackKing = nullptr;

    /* Create pieces according to the placement, from top to bottom, right to left, starting from the Top-Left corner */
    for (char c : piecePlacement) {
        
        if(c == '/') { /* Change row */
            col = 0;
            row--;
        }
        
        else if (isdigit(c)) /* Skip columns */
        {
            if((c - '0') > 0 && (c - '0') < 9) {
                int emptySquares = c - '0';
                col += emptySquares;
                if(col > (COL - 1)) col = COL - 1; /* Prevent overflow */
            }
        }

        else /* Create a piece */
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

    /* Store turn information into game */
    Color turn = (activeColor == "w") ? Color::White :
    (activeColor == "b") ? Color::Black : Color::White;
    game->setTurn(turn);

    if (!whiteKing || !blackKing) {
        throw std::invalid_argument("There is no one or both of Kings on the Board!");
    }
    
    /* Castling Rights of white and black kings */
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

    /* enPassant target square */
    if (enPassant == "-") {
        enPassantIndex = -1;
    } else {
        try {
            enPassantIndex = Board::algebraicToIndex(enPassant);
        } catch (std::invalid_argument) {
            std::cerr << "Invalid en-passant target square. Using default value instead" << std::endl;
            enPassantIndex = -1;
        }
    }

    /* Set Half Move Clock */
    if (std::stoi(halfMoveClock) >= 0) {
        game->setHalfMoveClock(std::stoi(halfMoveClock));
    } else {
        
    }
    

    /* Set Full Move Clock */
    if (std::stoi(fullMoveClock) >= 1) {
        game->setFullMoveClock(std::stoi(fullMoveClock));
    }
    

    /* Precompute valid pseudomoves and attackboards */
    computeAllMoves();
    computeAttackBoards();    

    /* Precompute moves for Kings */
    whiteKing->computeMoves();
    blackKing->computeMoves();

    return true;
}

/* Check if the given index is within the defined boundaries */
bool Board::isValidIndex(int index) {
    return index >= 0 && index < 64;
}

/* Compares the origin square to the destination square and returns if it is Empty, Invalid, Friendly or Enemy */
SquareStatus Board::getSquareStatus(int fromIndex, int toIndex) const {
    
    if (board[fromIndex] == nullptr) return SquareStatus::Invalid;
    if (!isValidIndex(toIndex)) return SquareStatus::Invalid;
    if (!isValidIndex(fromIndex)) return SquareStatus::Invalid;
    if(board[toIndex] == nullptr) return SquareStatus::Empty; /* empty */
    if(board[fromIndex]->getColor() != board[toIndex]->getColor()) return SquareStatus::Enemy; /* different piece color */
    if(board[fromIndex]->getColor() == board[toIndex]->getColor()) return SquareStatus::Friendly;

    return SquareStatus::Invalid;
}

/* Computes the squares that are being attacked by the set of pieces of the same color, based on the pseudomoves */
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

/* This method generates all pseudolegal moves, i.e., for every piece in the Board */
void Board::computeAllMoves() {
    
    /* Compute moves for each piece */
    for (int i = 0; i < 64; i++) {
        if (board[i] != nullptr) {
            board[i]->validMoves.clear();
            board[i]->computeMoves();
        } 
    }
   
}

/* Validates the pseudomoves of a Piece in a position */
void Board::validateMovesForPiece(int index) {
    if (!isValidIndex(index)) return;
    Piece * piece = board[index];
    if (!piece) return;

    // Get all pseudolegal moves first
    std::vector<int> pseudolegalMoves;
    //piece->computeMoves();  // Generates pseudolegal moves
    pseudolegalMoves = piece->validMoves;
    std::vector<int> legalMoves;
    
    /* Validate each move by calling validateMove method */
    for (int target : pseudolegalMoves) {
        if(validateMove(index, target)) {
            legalMoves.push_back(target);
        }
    }
    
    /* Copy all the legalMoves to the piece validMoves vector */
    piece->validMoves = legalMoves;
}

/* Method responsible for VALIDATING a move, which means it considers if the move is performed, would it lead the King (of same color) be in check. If yes, then the move is illegal, if not, the move is legal. This allows for the existance of pinned pieces, double checks, forks, and checkmate/stalemate detection (when there are no legal moves remaining) */
bool Board::validateMove(int fromIndex, int toIndex) {

    /* Checks if the indexes are within the bounds */
    if (!isValidIndex(fromIndex) || !isValidIndex(toIndex))
        return false;
    /* Check if the moving piece exists */
    Piece* movingPiece = board[fromIndex];

    if (!movingPiece) return false;

    /* Prevent KING CAPTURE */
    Piece * targetPiece = board[toIndex];
    if (targetPiece && targetPiece->getType() == PieceType::King)
        return false;

    /* Create a temporary board for validation, by copying the entire board. Yes, it is a bit expensive, but it is the easier way of validating a move */
    Board tempBoard(*this);
    
    /* Simulate the move on temporary board */
    tempBoard.movePiece(fromIndex, toIndex);
    
    if (tempBoard.isKingInCheck(movingPiece->getColor())) {
        return false; /* Move would lead the King being checked, which means it is an ILLEGAL move */
    } else {
        return true;
    }

}

bool Board::movePiece(int fromIndex, int toIndex) {
    
    /* Checks if the indexes are within the bounds */
    if (!isValidIndex(fromIndex) || !isValidIndex(toIndex))
        return false;
    Piece* movingPiece = board[fromIndex];
    if (!movingPiece) return false;

    /* Determine move type */
    bool normalMove = true;
    bool enPassantCapture = false;
    if (movingPiece->getType() == PieceType::Pawn && toIndex == enPassantIndex) {
        enPassantCapture = true;
        normalMove = true; /* Because it moves normally */
    }

    bool castlingMove = false;
    if (movingPiece->getType() == PieceType::King && std::abs(toIndex - fromIndex) == 2) {
        if (movingPiece->getColor() == Color::White) castlingMove = whiteKing->hasCastleRights();
        else {castlingMove = blackKing->hasCastleRights();}
        
        if (castlingMove) normalMove = false;
    }

    bool pawnPromotion = false;
    if (movingPiece->getType() == PieceType::Pawn) {
        if (movingPiece->getColor() == Color::White && indexToRow(toIndex) == 7) pawnPromotion = true;
        if (movingPiece->getColor() == Color::Black && indexToRow(toIndex) == 0) pawnPromotion = true;
    }

    /* Prevent KING CAPTURE */
    Piece * targetPiece = board[toIndex];
    if (targetPiece && targetPiece->getType() == PieceType::King)
        return false;

    /* Delete a piece if it is not a King or not empty */
    if (targetPiece && targetPiece->getType() != PieceType::King) {
        delete targetPiece;
    }

    if (enPassantCapture) {
        int capturedPawnIndex = (movingPiece->getColor() == Color::White) ? toIndex - 8 : toIndex + 8;
        delete board[capturedPawnIndex];          // Delete the pawn
        board[capturedPawnIndex] = nullptr;       // Clear the pointer
    }

    if (normalMove) {
        board[fromIndex] = nullptr;
        board[toIndex] = movingPiece;
        movingPiece->setPosition(toIndex);
        movingPiece->setHasMoved(true);
    }

    if(pawnPromotion) {
        Color color = movingPiece->getColor();
        delete board[toIndex];
        board[toIndex] = createPiece(PieceType::Queen, color, toIndex);
    }
    
    if(castlingMove) {
        Piece * rook = nullptr;
        /* King Side Castling */
        if (toIndex - fromIndex == 2) {
            board[fromIndex] = nullptr;
            board[fromIndex + 2] = movingPiece;
            rook = board[fromIndex + 3];
            board[fromIndex + 3] = nullptr;
            board[fromIndex + 1] = rook;

            movingPiece->setHasMoved(true); rook->setHasMoved(true);
            movingPiece->setPosition(toIndex); rook->setPosition(toIndex - 1);
        }

        /* Queen Side Castling */
        if (toIndex - fromIndex == -2) {
            board[fromIndex] = nullptr;
            board[fromIndex - 2] = movingPiece;
            rook = board[fromIndex - 4];
            board[fromIndex - 4] = nullptr;
            board[fromIndex - 1] = rook;

            movingPiece->setHasMoved(true); rook->setHasMoved(true);
            movingPiece->setPosition(toIndex); rook->setPosition(toIndex + 1);
        }
    }

    /* Update king pointers if necessary */
    if (movingPiece->getType() == PieceType::King) {
        if (movingPiece->getColor() == Color::White) {
            whiteKing = static_cast<King*>(movingPiece);
        } else {
            blackKing = static_cast<King*>(movingPiece);
        }
    }

    /* Generate the enPassantIndex if a pawn moved twice */
    if (movingPiece && movingPiece->getType() == PieceType::Pawn) {
        int distance = toIndex - fromIndex;
        if (distance == 16 || distance == -16) {
            if (movingPiece->getColor() == Color::White) enPassantIndex = toIndex - 8;
            else enPassantIndex = toIndex + 8;
            
        } else {
            enPassantIndex = -1;
        }
    } else {
        enPassantIndex = -1;
    }
   
    /* Recompute all the moves, recompute the attacking boards and recompute once again for the Kings */
    computeAllMoves();
    computeAttackBoards();
    whiteKing->computeMoves();
    blackKing->computeMoves();

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

/* Checks if the position of the King of a color is being attacked (marked as 1) in the attacking board of the opposite color */
bool Board::isKingInCheck(Color color) {
    King * king = (color == Color::White) ? static_cast<King *>(whiteKing) : static_cast<King *>(blackKing);
    if (king == nullptr) return false;
    king->setCheck(false);

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
    return isAttacked;
}

/* Check if there are any legal moves to the pieces of a Color. It is used to detect checkmate or stalemate */
bool Board::existLegalMoves(Color color) {
    King * king = (color == Color::White) ? whiteKing : blackKing;
    /* Check for valid king moves*/
    validateMovesForPiece(king->getPosition());
    if (!king->validMoves.empty()) {
        return true;
    }

    /* If there are no legal moves, then loop through friendly pieces and check if they have any valid move */
    for (int i = 0; i < 64; ++i) {
        if (board[i] && board[i]->getColor() == color) {
            validateMovesForPiece(i);
            if (!board[i]->validMoves.empty()) {
                return true;;
            }
        }
    }

    return false;
}