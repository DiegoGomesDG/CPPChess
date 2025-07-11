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
Board::Board(ChessGame * game) : mGamePtr(game), mEnPassantIndex(-1) {
    /* Set every pointer of board to NULL, ocupation board to 0 */
    for (int i = 0; i < ROW * COL; ++i) {
        board[i] = nullptr;
        whiteAttackBoard[i] = 0;
        blackAttackBoard[i] = 0;
    }
    mWhiteKing = nullptr;
    mBlackKing = nullptr;
}

/* Copy Constructor, which will copy every piece of the Board with new addresses, so move simulations can be performed */
Board::Board(const Board & original) {
     // Deep copy pieces
     for (int i = 0; i < 64; ++i) {
        board[i] = original.board[i] ? original.board[i]->clone(this) : nullptr;
        if (board[i] && board[i]->getType() == PieceType::King) {
            if (board[i]->getColor() == Color::White) mWhiteKing = static_cast<King *>(board[i]);
            else mBlackKing = static_cast<King *>(board[i]);
        }
    }
    // Copy attack boards
    whiteAttackBoard = original.whiteAttackBoard;
    blackAttackBoard = original.blackAttackBoard;

    /* Copying enPassant */
    mEnPassantIndex = original.getEnPassantIndex();

    /* Game Pointer is not necessary */
    mGamePtr = nullptr;
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
    mWhiteKing = nullptr;
    mBlackKing = nullptr;
    mEnPassantIndex = -1;
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
    int stringIndex;

    /* Initialize pointers to the white and black kings */
    mWhiteKing = nullptr;
    mBlackKing = nullptr;

    /* Create pieces according to the placement, from top to bottom, right to left, starting from the Top-Left corner */
    for (char character : piecePlacement) {
        
        if(character == '/') { /* Change row */
            col = 0;
            row--;
        }
        
        else if (isdigit(character)) /* Skip columns */
        {
            if((character - '0') > 0 && (character - '0') < 9) {
                int emptySquares = character - '0';
                col += emptySquares;
                if(col > (COL - 1)) col = COL - 1; /* Prevent overflow */
            }
        }

        else /* Create a piece */
        {
            Color color = (isupper(character)) ? Color::White : Color::Black;
            PieceType type;
            character = tolower(character);
            switch(character) {    
                case 'k': type = PieceType::King; break;
                case 'q': type = PieceType::Queen; break;
                case 'r': type = PieceType::Rook; break;
                case 'n': type = PieceType::Knight; break;
                case 'b': type = PieceType::Bishop; break;
                case 'p': type = PieceType::Pawn; break;
                default: return false; /* Invalid Piece */
            }
            stringIndex = squareToIndex(row, col);
            board[stringIndex] = createPiece(type, color, stringIndex);
            
            /* Two square moving rights for pawns */
            
            if (type == PieceType::Pawn) {
                int initialRow = (color == Color::White) ? 1 : 6;
                Pawn * pawn = static_cast<Pawn *>(board[stringIndex]);
                if (row == initialRow) {
                    pawn->setHasMoved(false);
                } else {
                    pawn->setHasMoved(true);
                }
            }

            if (type == PieceType::King && color == Color::White) mWhiteKing = static_cast<King *>(board[stringIndex]);
            if (type == PieceType::King && color == Color::Black) mBlackKing = static_cast<King *>(board[stringIndex]);
            ++col;
            if (col > (COL - 1)) col = COL;
        }
    }

    /* Store turn information into game */
    Color playerTurn = (activeColor == "w") ? Color::White :
    (activeColor == "b") ? Color::Black : Color::White;
    mGamePtr->setTurn(playerTurn);

    if (!mWhiteKing || !mBlackKing) {
        throw std::invalid_argument("There is no one or both of Kings on the Board!");
    }
    
    /* Castling Rights of white and black kings */
    if (mWhiteKing && mBlackKing) {
        if (castlingRights.find('K') != std::string::npos) {
            static_cast<King *>(mWhiteKing)->setKingSideCastleRight(true);
            if(board[7] != nullptr) board[7]->setHasMoved(false);
        }
        if(castlingRights.find('Q') != std::string::npos) {
            static_cast<King *>(mWhiteKing)->setQueenSideCastleRight(true);
            if(board[0] != nullptr) board[0]->setHasMoved(false);
        }
        if(castlingRights.find('k') != std::string::npos) {
            static_cast<King *>(mBlackKing)->setKingSideCastleRight(true);
            if(board[63] != nullptr) board[63]->setHasMoved(false);
        }
        if(castlingRights.find('q') != std::string::npos) {
            static_cast<King *>(mBlackKing)->setQueenSideCastleRight(true);
            if(board[56] != nullptr) board[56]->setHasMoved(false);
        }
    } else return false;

    /* enPassant target square */
    if (enPassant == "-") {
        mEnPassantIndex = -1;
    } else {
        try {
            mEnPassantIndex = Board::algebraicToIndex(enPassant);
        } catch (std::invalid_argument) {
            std::cerr << "Invalid en-passant target square. Using default value instead" << std::endl;
            mEnPassantIndex = -1;
        }
    }

    /* Set Half Move Clock */
    if (std::stoi(halfMoveClock) >= 0) {
        mGamePtr->setHalfMoveClock(std::stoi(halfMoveClock));
    } else {
        
    }
    
    /* Set Full Move Clock */
    if (std::stoi(fullMoveClock) >= 1) {
        mGamePtr->setFullMoveClock(std::stoi(fullMoveClock));
    }
    
    /* 1. Compute all non-king moves first */
    for (int i = 0; i < 64; ++i) {
        if (board[i] && board[i]->getType() != PieceType::King) {
            board[i]->computeMoves();
        }
    }

    /* 2. Compute attack boards (now excluding kings) */
    computeAttackBoards();

    /* 3. Compute king moves using updated attack boards */
    if (mWhiteKing) mWhiteKing->computeMoves();
    if (mBlackKing) mBlackKing->computeMoves();

    /* 4. Final attack board update including kings */
    computeAttackBoards();

    /* 5. Validate all valid moves after loading a position */
    validateAllNextPlayerMoves(playerTurn);
    countMoves(playerTurn);

    /* Check detection */
    mWhiteKing->setCheck(isKingInCheck(Color::White));
    mBlackKing->setCheck(isKingInCheck(Color::Black));   

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

    // 1. Compute all non-king pieces' attacks
    for (int i = 0; i < 64; ++i) {
        if (board[i] && board[i]->getType() != PieceType::King) {
            board[i]->computeMoves();
            for (int target : board[i]->validMoves) {
                if (board[i]->getColor() == Color::White) {
                    whiteAttackBoard[target] = 1;
                    if (board[i]->getType() == PieceType::Pawn && (indexToColumn(board[i]->getPosition()) - indexToColumn(target)) == 0) whiteAttackBoard[target] = 0;
                } else {
                    blackAttackBoard[target] = 1;
                    if (board[i]->getType() == PieceType::Pawn && (indexToColumn(board[i]->getPosition()) - indexToColumn(target)) == 0) blackAttackBoard[target] = 0; 
                }
            }
        }
    }

    // 2. Compute kings' attacks (using existing attack boards)
    if (mWhiteKing) {
        mWhiteKing->computeMoves();  // Uses blackAttackBoard
        for (int target : mWhiteKing->validMoves) {
            whiteAttackBoard[target] = 1;
        }
    }
    
    if (mBlackKing) {
        mBlackKing->computeMoves();  // Uses whiteAttackBoard
        for (int target : mBlackKing->validMoves) {
            blackAttackBoard[target] = 1;
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
    std::vector<int> pseudoLegalMoves;
    //piece->computeMoves();  // Generates pseudolegal moves
    pseudoLegalMoves = piece->validMoves;
    std::vector<int> legalMoves;
    
    /* Validate each move by calling validateMove method */
    for (int targetIndex : pseudoLegalMoves) {
        if(validateMove(index, targetIndex)) {
            legalMoves.push_back(targetIndex);
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
    Piece * movingPiece = board[fromIndex];
    if (!movingPiece) return false;

    /* Prevent KING CAPTURE */
    Piece * targetPiece = board[toIndex];
    if (targetPiece && targetPiece->getType() == PieceType::King)
        return false;

    /* Create a temporary board for validation, by copying the entire board. Yes, it is a bit expensive, but it is the easier way of validating a move */
    Board tempBoard(*this);
    movingPiece = tempBoard.board[fromIndex];
    targetPiece = tempBoard.board[toIndex];
    
    /* #################### Simulate the move on temporary board #################### */
    /* Determine move type */
    bool normalMove = true;
    bool enPassantCapture = false;
    if (movingPiece->getType() == PieceType::Pawn && toIndex == tempBoard.mEnPassantIndex) {
        enPassantCapture = true;
        normalMove = true; /* Because it moves normally */
    }

    /* Delete a piece if it is not a King or not empty */
    if (targetPiece && targetPiece->getType() != PieceType::King) {
        delete targetPiece;
    }

    if (enPassantCapture) {
        int capturedPawnIndex = (movingPiece->getColor() == Color::White) ? toIndex - 8 : toIndex + 8;
        delete tempBoard.board[capturedPawnIndex];          // Delete the pawn
        tempBoard.board[capturedPawnIndex] = nullptr;       // Clear the pointer
    }

    if (normalMove) {
        tempBoard.board[fromIndex] = nullptr;
        tempBoard.board[toIndex] = movingPiece;
        movingPiece->setPosition(toIndex);
        movingPiece->setHasMoved(true);
    }

    /* Compute attack boards */
    tempBoard.computeAllMoves();
    tempBoard.computeAttackBoards();

    /* Update king pointers if necessary */
    if (movingPiece->getType() == PieceType::King) {
        if (movingPiece->getColor() == Color::White) {
            tempBoard.mWhiteKing = static_cast<King*>(movingPiece);
        } else {
            tempBoard.mBlackKing = static_cast<King*>(movingPiece);
        }
    }
    
    if (tempBoard.isKingInCheck(movingPiece->getColor())) {
        return false; /* Move would lead the King being checked, which means it is an ILLEGAL move */
    } else {
        return true;
    }

}

void Board::validateAllNextPlayerMoves(Color turn) {

    // Validate all pieces of the next player
    for (int i = 0; i < 64; i++) {
        if (board[i] && board[i]->getColor() == turn) {
            validateMovesForPiece(i);
        }
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
    if (movingPiece->getType() == PieceType::Pawn && toIndex == mEnPassantIndex) {
        enPassantCapture = true;
        normalMove = true; /* Because it moves normally */
    }

    bool castlingMove = false;
    if (movingPiece->getType() == PieceType::King && std::abs(toIndex - fromIndex) == 2) {
        if (movingPiece->getColor() == Color::White) castlingMove = mWhiteKing->hasCastleRights() && mWhiteKing->isChecked();
        else {castlingMove = mBlackKing->hasCastleRights() && mBlackKing->isChecked();}
        
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
            mWhiteKing = static_cast<King*>(movingPiece);
        } else {
            mBlackKing = static_cast<King*>(movingPiece);
        }
    }

    /* Generate the enPassantIndex if a pawn moved twice */
    if (movingPiece && movingPiece->getType() == PieceType::Pawn) {
        int distance = toIndex - fromIndex;
        if (distance == 16 || distance == -16) {
            if (movingPiece->getColor() == Color::White) mEnPassantIndex = toIndex - 8;
            else mEnPassantIndex = toIndex + 8;
            
        } else {
            mEnPassantIndex = -1;
        }
    } else {
        mEnPassantIndex = -1;
    }
   
    /* Recompute all the moves, recompute the attacking boards and recompute once again for the Kings */
    computeAllMoves();
    computeAttackBoards();

    /* First update the kings' moves based on new attack boards */
    mWhiteKing->computeMoves();
    mBlackKing->computeMoves();
    computeAttackBoards(); /* Temporary Solution to the Problem! */


    /* Then evaluate check status based on current attack boards */
    bool whiteInCheck = isKingInCheck(Color::White);
    bool blackInCheck = isKingInCheck(Color::Black);

    /* Now safely update check status */
    mWhiteKing->setCheck(whiteInCheck);
    mBlackKing->setCheck(blackInCheck);
    
    return true;

}

void Board::countMoves(Color color) {
    moveCount = 0;
    for (int i = 0; i < 64; i++) {
        if (board[i] && board[i]->getColor() == color) {
            moveCount += board[i]->validMoves.size();
        }
    }
}

/* Checks if the position of the King of a color is being attacked (marked as 1) in the attacking board of the opposite color */
bool Board::isKingInCheck(Color color) {
    King * king = (color == Color::White) ? static_cast<King *>(mWhiteKing) : static_cast<King *>(mBlackKing);
    if (!king) return false;

    const int kingPos = king->getPosition();
    const bool isAttacked = (color == Color::White) ? blackAttackBoard[kingPos] : whiteAttackBoard[kingPos];

    if (isAttacked != king->isChecked()) {
        king->setCheck(isAttacked);
    }
    return isAttacked;
}

/* Check if there are any legal moves to the pieces of a Color. It is used to detect checkmate or stalemate */
bool Board::existLegalMoves(Color color) {
    King * king = (color == Color::White) ? mWhiteKing : mBlackKing;
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
                return true;
            }
        }
    }

    return false;
}