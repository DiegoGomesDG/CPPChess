/* Standard Libraries */
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

/* User Libraries */
#include "Game.hpp"
#include "Graphics.hpp"
#include "Piece.hpp"

/* SDL */
#include "SDL_events.h"
#include "SDL_mixer.h"


/* Game Loader */
ChessGame::ChessGame(const std::string& fen) : state(GameState::Idle), board(this), focusIndex(-1), targetIndex(-1), turn(Color::White), halfMoveClock(0), fullMoveClock(1), wasClicked(false) {
    board.loadFromFEN(fen); 
    graphics.clearWindow();
    graphics.loadMedia();
    graphics.renderBoardWithPieces(board);
    Mix_PlayChannel(-1, gameStartSound, 0);
}

/* Reset the Game */
void ChessGame::resetGame() {
    state = GameState::Idle; /* Reset */
    focusIndex = -1; /* Reset */
    targetIndex = -1; /* Reset */
    moveList.clear(); /* Clear the Move List */
    board.loadFromFEN(); /* Load Default Board */
    graphics.renderBoardWithPieces(board); /* Render */
}

/* Works on basis of a High Level State Machine (Digital Design 1, 1st Semester) */
/* void ChessGame::handleStateTransition() {

    switch(state) {

        case GameState::Idle:
        
        case GameState::PieceSelected:

        case GameState::Dragging:

        case GameState::Processing:
            
        case GameState::GameOver:

    }
} */

/* Handle the different events */
void ChessGame::handleEvent(SDL_Event & event) {
    SDL_Point mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    switch(event.type) {
    
    case SDL_QUIT:
        state = GameState::Quit;
        return;

    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_f) {
            graphics.flipBoard();
            graphics.renderBoardWithPieces(board);
            break;
        }

    case SDL_MOUSEMOTION:
        mousePos = {event.motion.x, event.motion.y};
        if(leftMouseButtonDown && state == GameState::PieceSelected && focusIndex != -1) {
            state = GameState::Dragging;
            wasClicked = false;
        }

        if(state == GameState::Dragging && focusIndex != -1) {
            graphics.renderDraggedPiece(board, focusIndex, mousePos.x, mousePos.y);
        }
        break;

    case SDL_MOUSEBUTTONDOWN:
        if (!leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) {
            leftMouseButtonDown = true;

            int col = (mousePos.x - BORDER_SIZE) / SQUARE_SIZE;
            int row = (mousePos.y - BORDER_SIZE) / SQUARE_SIZE;

            if (mousePos.x > BORDER_SIZE && mousePos.x < WIN_WIDTH - BORDER_SIZE && mousePos.y > BORDER_SIZE && mousePos.y < WIN_HEIGHT - BORDER_SIZE) {
                int clickedIndex = -1;
                if (!graphics.isBoardFlipped) {
                    clickedIndex = Board::squareToIndex((ROW - 1) - row, col);
                } else {
                    clickedIndex = Board::squareToIndex(row, (COL - 1) - col);
                }

                if (state == GameState::Idle) {
                    Piece * piece = board.board[clickedIndex];
                    if (piece && piece->getColor() == turn) {
                        focusIndex = clickedIndex;
                        state = GameState::PieceSelected;
                        board.validateMovesForPiece(focusIndex);
                        graphics.selectPiece(board, focusIndex);
                    }
                } else if (state == GameState::PieceSelected) {
                    targetIndex = clickedIndex;
                    if (targetIndex == focusIndex) {
                        /* Clicked Same Square Again */
                        //state = GameState::Idle;
                        //focusIndex = -1;
                        //targetIndex = -1;
                        //graphics.renderBoardWithPieces(board);
                    } else {
                        Piece * focusedPiece = board.board[focusIndex];
                        Piece * targetPiece = board.board[targetIndex];
                        
                        if (targetPiece && targetPiece->getColor() == turn) {
                            focusIndex = targetIndex;
                            board.validateMovesForPiece(focusIndex);
                            graphics.selectPiece(board, focusIndex);
                            return;
                        }
                        if (focusedPiece && focusedPiece->isValidMove(targetIndex)) {
                            state = GameState::Processing;
                            wasClicked = true;
                            //if(targetPiece != nullptr) {
                            //    Mix_PlayChannel(-1, captureSound, 0);
                            //} else Mix_PlayChannel(-1, moveSound, 0);
                        } else {
                            /* Invalid move or empty square -> deselect */
                            state = GameState::Idle;
                            focusIndex = -1;
                            targetIndex = -1;
                            graphics.renderBoardWithPieces(board);
                        }
                    }
                }
            }
        }
        break;        

    case SDL_MOUSEBUTTONUP:
        if(leftMouseButtonDown && event.button.button == SDL_BUTTON_LEFT) {
            leftMouseButtonDown = false;
            
            int dropCol = (mousePos.x - BORDER_SIZE) / SQUARE_SIZE;
            int dropRow = (mousePos.y - BORDER_SIZE) / SQUARE_SIZE;

            if(mousePos.x < (WIN_WIDTH + BORDER_SIZE) && (mousePos.x > BORDER_SIZE) && mousePos.y < (WIN_HEIGHT + BORDER_SIZE) && (mousePos.y > BORDER_SIZE)) {
                if (!graphics.isBoardFlipped) {
                    targetIndex = Board::squareToIndex((ROW - 1) - dropRow, dropCol);
                } else {
                    targetIndex = Board::squareToIndex(dropRow, (COL - 1) - dropCol);
                }
                
            } else {
                targetIndex = -1;
            }

            if (state == GameState::Dragging && focusIndex != -1) {
                Piece * piece = board.board[focusIndex];
                
                if (targetIndex != -1 && piece != nullptr && piece->isValidMove(targetIndex)) {
                    state = GameState::Processing;
                } else {
                    /* Invalid Drop, return to previous selection */
                    state = GameState::PieceSelected;
                    graphics.selectPiece(board, focusIndex);
                    Mix_PlayChannel(-1, illegalMoveSound, 0);
                }
            }
        }
        break;

    }

    if (state == GameState::Processing && focusIndex != -1 && targetIndex != -1) {
        Piece * focusedPiece = board.board[focusIndex];
        Piece * targetPiece = board.board[targetIndex];
        if (board.validateMove(focusIndex, targetIndex)) {
            /* Animate the move if it was a click (not drag) */
            if (wasClicked) {
                graphics.animatePieceMoving(board, focusIndex, targetIndex);
                wasClicked = false;
            }

            /* Play the capture sound if it is an en-passant */
            if(targetIndex == board.getEnPassantIndex())
                Mix_PlayChannel(-1, captureSound, 0);

            /* Register Move */
            registerMove();

            /* Execute the move on the logical board */
            if (board.movePiece(focusIndex, targetIndex)) {
                // Switch turns
                turn = (turn == Color::White) ? Color::Black : Color::White;

                if(board.isKingInCheck(turn)) {
                    Mix_PlayChannel(-1, moveCheckSound, 0);
                } else {
                    if(targetPiece != nullptr) {
                        Mix_PlayChannel(-1, captureSound, 0);
                    } else if (abs(targetIndex - focusIndex) == 2 && focusedPiece->getType() == PieceType::King) {
                        Mix_PlayChannel(-1, castleSound, 0);
                    } else {
                        Mix_PlayChannel(-1, moveSound, 0);
                    }
                }
                // Update the board display
                graphics.renderBoardWithPieces(board);
            } else {
                // Handle invalid move (should not happen as we validated earlier)
                Mix_PlayChannel(-1, illegalMoveSound, 0);
            }
        } else {
            // Handle invalid move
            Mix_PlayChannel(-1, illegalMoveSound, 0);
        }

        /* Increment Full Move Clock */
        if (turn == Color::Black) {
            ++fullMoveClock;
        }

        /* Check if it is a Checkmate */
        if (!board.existLegalMoves(turn)) {
            if (board.isKingInCheck(turn)) {
                state = GameState::GameOver;
            }
        }

        // Reset state
        if (state != GameState::GameOver) {
            state = GameState::Idle;
        }
        std::cerr << moveList.back() << std::endl;
        focusIndex = -1;
        targetIndex = -1;
        
    }

    if (state == GameState::GameOver) {
        std::string outcome;
        std::string inMoveList;
        if (board.isKingInCheck(turn)) {
            outcome = (turn == Color::White) ? "Black Wins by Checkmate" : "White Wins by Checkmate";
            inMoveList = (turn == Color::White) ? "0-1" : "1-0";
        } else {
            outcome = "Stalemate";
            inMoveList = "1/2-1/2";
        }
        graphics.printText(board, outcome);
        moveList.push_back(inMoveList);
        generatePGN(inMoveList);
        SDL_Delay(5000);
        resetGame();
    }

}

/* Register a Move. Partially implemented, and is not totally according to the Standard Algebraic Notation */
void ChessGame::registerMove() {
    std::string move;
    Piece * movingPiece = board.board[focusIndex];
    Piece * targetSquare = board.board[targetIndex];
    if (movingPiece == nullptr) return; /* Throw?? */

    int fromIndex = focusIndex;
    int destIndex = targetIndex;

    /* Add Numbering for White Moves */
    if (turn == Color::White) {
        /* https://stackoverflow.com/questions/5590381/how-to-convert-int-to-string-in-c*/
        std::string number = std::to_string(fullMoveClock) + ".";
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

    oss << "games/" << std::put_time(&tm, "%Y-%m-%d_%H-%M-%S") << ".pgn";
    dateoss << std::put_time(&tm, "%Y.%m.%d");

    std::string filename = oss.str();
    std::string date = dateoss.str();

    /* Create File */
    std::fstream pgn(filename, std::ios::out);

    /* Write Tags */
    pgn << "[Event \"Two Player Chess\"]" << std::endl;
    pgn << "[Site \"CPPChess Project\"]" << std::endl;
    pgn << "[Date \"" << date << "\"]" << std::endl;
    pgn << "[Result " << result << "\"]";
    pgn << std::endl; /* Separation */

    /* Write all the moves in the Vector */
    for (std::string & move : moveList) {
        pgn << move << " ";
    }

}