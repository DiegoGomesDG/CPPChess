/* Standard Libraries */
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <fstream>

#include <unistd.h>

/* User Libraries */
#include "Game.hpp"
#include "Graphics.hpp"
#include "Piece.hpp"

/* SDL */
#include "SDL_events.h"
#include "SDL_mixer.h"
#include "SDL_timer.h"

/* Game Loader */
ChessGame::ChessGame(const std::string& fen) : mState(GameState::Idle), mBoard(this), mFocusIndex(-1), mTargetIndex(-1), mTurn(Color::White), mHalfMoveClock(0), mFullMoveClock(1), mWasClicked(false), mProcessGameOver(false) {
    try {
        mBoard.loadFromFEN(fen); 
    }
    catch (std::invalid_argument) {
        std::cerr << "Invalid FEN. Board initialized using the default initial position" << std::endl;
        mBoard.loadFromFEN();
    }

    graphics.loadMedia();
    graphics.renderBoardWithPieces(mBoard);
    Mix_PlayChannel(-1, gameStartSound, 0);
}

/* Reset the Game */
void ChessGame::resetGame() {
    mState = GameState::Idle; /* Reset */
    mFocusIndex = -1; /* Reset */
    mTargetIndex = -1; /* Reset */
    mProcessGameOver = false;
    moveList.clear(); /* Clear the Move List */
    mBoard.loadFromFEN(); /* Load Default Board */
    //graphics.renderBoardWithPieces(board); /* Render */
}

/* Apply a reset and start with a given FEN */
bool ChessGame::loadFEN(const std::string fen) {
    mState = GameState::Idle; /* Reset */
    mFocusIndex = -1; /* Reset */
    mTargetIndex = -1; /* Reset */
    mProcessGameOver = false;
    moveList.clear(); /* Clear the Move List */

    try {
        mBoard.loadFromFEN(fen); 
    }
    catch (std::invalid_argument) {
        std::cerr << "Invalid FEN. Board initialized using the default initial position" << std::endl;
        mBoard.loadFromFEN();
    }

    //graphics.renderBoardWithPieces(board); /* Render */
    return true;
}

/* Handle Render according to different States */
void ChessGame::handleRender() {
    switch(mState) {
        case GameState::Idle: graphics.renderBoardWithPieces(mBoard); break;
        case GameState::PieceSelected: graphics.selectPiece(mBoard, mFocusIndex); break;
        case GameState::Dragging: graphics.renderDraggedPiece(mBoard, mFocusIndex, mMousePos.x, mMousePos.y); break;
        case GameState::GameOver: graphics.renderBoardWithPieces(mBoard); break;
        default: break;
    }
}

void ChessGame::handleStatesProcessing() {
    switch(mState) {
        case GameState::Processing: handleProcessingMove(); break;
        case GameState::GameOver: handleGameOver(); break;
        default: break;
    }
}

/* Handle the different events */
void ChessGame::handleEvent(SDL_Event & event) {
    
    SDL_GetMouseState(&mMousePos.x, &mMousePos.y);
    switch(event.type) {

    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_f) {
            graphics.flipBoard();
            break;
        }

    case SDL_MOUSEMOTION:
        mMousePos = {event.motion.x, event.motion.y};
        if(mLeftMouseButtonDown && mState == GameState::PieceSelected && mFocusIndex != -1) {
            mState = GameState::Dragging;
            mWasClicked = false;
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (!mLeftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) {
            mLeftMouseButtonDown = true;

            int col = (mMousePos.x - BORDER_SIZE) / SQUARE_SIZE;
            int row = (mMousePos.y - BORDER_SIZE) / SQUARE_SIZE;

            if (mMousePos.x > BORDER_SIZE && mMousePos.x < WIN_WIDTH - BORDER_SIZE && mMousePos.y > BORDER_SIZE && mMousePos.y < WIN_HEIGHT - BORDER_SIZE) {
                int clickedIndex = -1;
                if (!graphics.isBoardFlipped) {
                    clickedIndex = Board::squareToIndex((ROW - 1) - row, col);
                } else {
                    clickedIndex = Board::squareToIndex(row, (COL - 1) - col);
                }

                /* If it is idle, it means the next state will be piece select */
                if (mState == GameState::Idle) {
                    Piece * piece = mBoard.board[clickedIndex];
                    if (piece && piece->getColor() == mTurn) {
                        mFocusIndex = clickedIndex;
                        mState = GameState::PieceSelected;
                    }

                /* If it is piece selected, then there is multiple possibilities */
                } else if (mState == GameState::PieceSelected) {
                    mTargetIndex = clickedIndex;
                    Piece * focusedPiece = mBoard.board[mFocusIndex];
                    Piece * targetPiece = mBoard.board[mTargetIndex];
                    
                    /* If is the same color piece, change the focus to the new focused piece*/
                    if (targetPiece && targetPiece->getColor() == mTurn) {
                        mFocusIndex = mTargetIndex;
                        return;
                    }

                    /* If the focused piece is not null and it is one of the valid moves, then change to move processing, otherwise deselect */
                    if (focusedPiece && focusedPiece->isValidMove(mTargetIndex)) {
                        mState = GameState::Processing;
                        mWasClicked = true;
                        //graphics.animatePieceMoving(board, focusIndex, targetIndex);

                    /* Invalid move or empty square -> deselect */
                    } else {
                        mState = GameState::Idle;
                        mFocusIndex = -1;
                        mTargetIndex = -1;
                    }
                }
            }
        }
        break;        

    case SDL_MOUSEBUTTONUP:
        if(mLeftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) {
            mLeftMouseButtonDown = false;
            
            int dropCol = (mMousePos.x - BORDER_SIZE) / SQUARE_SIZE;
            int dropRow = (mMousePos.y - BORDER_SIZE) / SQUARE_SIZE;

            if(mMousePos.x < (WIN_WIDTH + BORDER_SIZE) && (mMousePos.x > BORDER_SIZE) && mMousePos.y < (WIN_HEIGHT + BORDER_SIZE) && (mMousePos.y > BORDER_SIZE)) {
                if (!graphics.isBoardFlipped) {
                    mTargetIndex = Board::squareToIndex((ROW - 1) - dropRow, dropCol);
                } else {
                    mTargetIndex = Board::squareToIndex(dropRow, (COL - 1) - dropCol);
                }

            } else {
                mTargetIndex = -1;
            }

            if (mState == GameState::Dragging && mFocusIndex != -1) {
                Piece * piece = mBoard.board[mFocusIndex];
                if (mTargetIndex != -1 && piece != nullptr && piece->isValidMove(mTargetIndex)) {
                    mState = GameState::Processing;
                } else {
                    /* Invalid Drop, return to previous selection */
                    mState = GameState::PieceSelected;
                    if (mBoard.isKingInCheck(mTurn)) Mix_PlayChannel(-1, illegalMoveSound, 0);
                }
            }
        }
        break;

    }
}

/* Handle all processes related to a move and checking for game end conditions */
void ChessGame::handleProcessingMove() {

    /* If one of the indexes is not valid, do not proceed with the handling */
    if (!(Board::isValidIndex(mFocusIndex) && Board::isValidIndex(mTargetIndex)))
        return;

    Piece * focusedPiece = mBoard.board[mFocusIndex];
    Piece * targetPiece = mBoard.board[mTargetIndex];
    if (mBoard.validateMove(mFocusIndex, mTargetIndex)) {

        /* Animate the move if it was a click (not drag) */
        if (mWasClicked) {
            graphics.animatePieceMoving(mBoard, mFocusIndex, mTargetIndex); /* Try to remove from here! */
            mWasClicked = false;
        }

        /* Play the capture sound if it is an en-passant */
        if(mTargetIndex == mBoard.getEnPassantIndex())
            Mix_PlayChannel(-1, captureSound, 0);

        /* Increment halfMoveClock */
        mHalfMoveClock++;

        /* Reset halfMoveClock if it is a pawn or a capture */
        if (focusedPiece->getType() == PieceType::Pawn || (targetPiece != nullptr)) {
            mHalfMoveClock = 0;
        }

        /* Register Move */
        registerMove();

        /* Execute the move on the logical board */
        if (mBoard.movePiece(mFocusIndex, mTargetIndex)) {

            /* Switch turns */
            mTurn = (mTurn == Color::White) ? Color::Black : Color::White;
            mBoard.validateAllNextPlayerMoves(mTurn);
            mBoard.countMoves(mTurn);

            /* Manage Sounds Effects :) */
            if(mBoard.isKingInCheck(mTurn)) {
                Mix_PlayChannel(-1, moveCheckSound, 0);
            } else {
                if(targetPiece != nullptr) {
                    Mix_PlayChannel(-1, captureSound, 0);
                } else if (abs(mTargetIndex - mFocusIndex) == 2 && focusedPiece->getType() == PieceType::King) {
                    Mix_PlayChannel(-1, castleSound, 0);
                } else {
                    Mix_PlayChannel(-1, moveSound, 0);
                }
            }
            // Update the board display
            graphics.renderBoardWithPieces(mBoard);
        }
    } else {
        if (mBoard.isKingInCheck(mTurn)) Mix_PlayChannel(-1, illegalMoveSound, 0);
    }

    /* Increment Full Move Clock */
    if (mTurn == Color::Black) {
        ++mFullMoveClock;
    }

    /* Check if it is a Checkmate */
    if (!mBoard.existLegalMoves(mTurn)) {
        mState = GameState::GameOver;
    }

    /* Check for 50 Move Rule */
    if (mHalfMoveClock == 50) {
        mState = GameState::GameOver;
    }

    // Reset state
    if (mState != GameState::GameOver) {
        mState = GameState::Idle;
    }
    std::cerr << moveList.back() << std::endl;
    mFocusIndex = -1;
    mTargetIndex = -1;
}

/* Handling GameOver. Prints the text into the screen, generates the PGN file and then resets the game */
void ChessGame::handleGameOver() {
    if (!mProcessGameOver) {
        std::string outcome;
        std::string inMoveList;

        if (mBoard.isKingInCheck(mTurn)) {
            outcome = (mTurn == Color::White) ? "Black Wins by Checkmate" : "White Wins by Checkmate";
            inMoveList = (mTurn == Color::White) ? "0-1" : "1-0";
        } else {
            if(mHalfMoveClock == 50) {
                outcome = "Draw by 50-Move Rule";
            } else {
                outcome = "Stalemate";
            }
            
            inMoveList = "1/2-1/2";
        }
        graphics.printText(mBoard, outcome);
        moveList.push_back(inMoveList);
        generatePGN(inMoveList);
        SDL_Delay(3000);
        mProcessGameOver = true;
    }

}

/* Register a Move. Partially implemented, and is not totally according to the Standard Algebraic Notation */
void ChessGame::registerMove() {
    std::string move;
    Piece * movingPiece = mBoard.board[mFocusIndex];
    Piece * targetSquare = mBoard.board[mTargetIndex];
    if (movingPiece == nullptr) return; /* Throw?? */

    int fromIndex = mFocusIndex;
    int destIndex = mTargetIndex;

    /* Add Numbering for White Moves */
    if (mTurn == Color::White) {
        /* https://stackoverflow.com/questions/5590381/how-to-convert-int-to-string-in-c*/
        std::string number = std::to_string(mFullMoveClock) + ".";
        move.append(number);
    }

    /* Pawn Moves */
    if (movingPiece->getType() == PieceType::Pawn) {
        move.append(Board::indexToAlgebraic(fromIndex));
        move.append(Board::indexToAlgebraic(destIndex));
        /* Add move and return */
        moveList.push_back(move);
        return;
    }

    /* Other Pieces */
    PieceType type = movingPiece->getType();
    char pieceID = '\0';
    switch (type) {
        case PieceType::Bishop: pieceID = 'B'; break;
        case PieceType::Knight: pieceID = 'N'; break;
        case PieceType::Rook: pieceID = 'R'; break;
        case PieceType::Queen: pieceID = 'Q'; break;
        case PieceType::King: pieceID = 'K'; break;
        default: break;
    }

    move += pieceID; /* Add the Piece Identifier */
    move.append(Board::indexToAlgebraic(fromIndex));
    if (targetSquare != nullptr) move += 'x'; /* Add x if it is a capture */
    move.append(Board::indexToAlgebraic(destIndex)); /* Add destination square*/
    moveList.push_back(move); /* add move to the list */

}

/* Initialize a PGN File (Not the full implementation for now). Asked to DeepSeek */
bool ChessGame::generatePGN(const std::string & result) {

    /* Create File Name using the Time */
    auto now = std::time(nullptr);
    std::tm tm = *std::localtime(&now);

    std::ostringstream oss;
    std::ostringstream dateoss;

    oss << "./games/" << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".pgn";
    dateoss << std::put_time(&tm, "%Y.%m.%d");

    std::string filename = oss.str();
    std::string date = dateoss.str();

    /* Check if the Vector is not empty */
    if (moveList.empty()) {
        return false;
    }

    try {
        /* Create File */
        std::fstream pgnFile;
        pgnFile.exceptions(std::ios::failbit | std::ios::badbit);
        pgnFile.open(filename, std::ios::out);

        /* Write Tags */
        pgnFile << "[Event \"Two Player Chess\"]" << std::endl;
        pgnFile << "[Site \"CPPChess Project\"]" << std::endl;
        pgnFile << "[Date \"" << date << "\"]" << std::endl;
        pgnFile << "[Result \"" << result << "\"]";
        pgnFile << std::endl; /* Separation */

        /* Write all the moves in the Vector */
        for (std::string & move : moveList) {
            pgnFile << move << " ";
        }

        pgnFile.close();
        return true;

    } catch (const std::exception& e) {
        std::cerr << "Failed to create or write PGN file: " << e.what() << std::endl;
        return false;
    }
    
}