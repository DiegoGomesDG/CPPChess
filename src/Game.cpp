#include "Game.hpp"
#include "Graphics.hpp"
#include "Piece.hpp"
#include "SDL_events.h"
#include "SDL_mixer.h"

ChessGame::ChessGame(const std::string& fen) : state(GameState::Idle), board(this), focusIndex(-1), targetIndex(-1), turn(Color::White), wasClicked(false) {
    board.loadFromFEN(fen); 
    graphics.clearWindow();
    graphics.loadMedia();
    graphics.renderBoardWithPieces(board);
    Mix_PlayChannel(-1, gameStartSound, 0);
}

/* Works on basis of a High Level State Machine (Digital Design 1, 1st Semester) */
void ChessGame::handleStateTransition() {
    
}

void ChessGame::handleEvent(SDL_Event & event) {
    SDL_Point mousePos;
    SDL_GetMouseState(&mousePos.x, &mousePos.y);

    switch(event.type) {
    
    case SDL_QUIT:
        state = GameState::GameOver;
        return;

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
                int clickedIndex = squareToIndex((ROW - 1) - row, col);

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
                targetIndex = squareToIndex((ROW - 1) - dropRow, dropCol);
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
            // Animate the move if it was a click (not drag)
            if (wasClicked) {
                graphics.animatePieceMoving(board, focusIndex, targetIndex);
                wasClicked = false;
            }

            /* Play the capture sound if it is an en-passant */
            if(targetIndex == board.getEnPassantIndex())
                Mix_PlayChannel(-1, captureSound, 0);


            // Execute the move on the logical board
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

        // Reset state
        state = GameState::Idle;
        focusIndex = -1;
        targetIndex = -1;

        std::string text = "White Wins";
        graphics.printStatusText(board, text);
    }

}

