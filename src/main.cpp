#include "../include/Graphics.hpp"
#include "../include/Board.hpp"
#include "../include/Texture.hpp"
#include "SDL_timer.h"

extern int SQUARE_SIZE;
extern int BORDER_SIZE;
extern int WIN_WIDTH;
extern int WIN_HEIGHT;

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
	//Start up SDL and create window
	Graphics boardGUI;
	boardGUI.loadMedia();
	//Clear screen
	boardGUI.clearWindow();
	boardGUI.renderBoard();

	Board board;
	std::string fen;
	if (argc > 1) {
		 fen = argv[1];
	} else {
		fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
	}

	board.loadFromFEN(fen);
	boardGUI.renderPieces(board);

	//Update screen
	boardGUI.updateWindow();

	SDL_Event e;
	bool quit = false;
	bool leftMouseButtonDown = false;
	SDL_Point mousePos;
	int focusRow, focusCol, dropRow, dropCol;
	int index;
	int dropIndex;
	bool executeOnce = true;
	dropIndex = -1;

	/* TO DO: Transform this into a method in class graphics, that returns the destination square of the piece */
	while(!quit) {
			//Handle events on queue
		while(SDL_PollEvent(&e) != 0) {

			if(e.type == SDL_QUIT) {
				quit = true;
			}

			if(e.type == SDL_MOUSEMOTION) {
				mousePos = {e.motion.x, e.motion.y};
				if(leftMouseButtonDown && board.board[index] != NULL) {
					boardGUI.renderDraggedPiece(board, index, mousePos.x, mousePos.y);
					
				}
			}

			if(e.type == SDL_MOUSEBUTTONUP) {
				if(leftMouseButtonDown && e.button.button == SDL_BUTTON_LEFT) {
					leftMouseButtonDown = false;
					mousePos = {e.motion.x, e.motion.y};
					if(mousePos.x < (WIN_WIDTH + BORDER_SIZE) && (mousePos.x > BORDER_SIZE) && mousePos.y < (WIN_HEIGHT + BORDER_SIZE) && (mousePos.y > BORDER_SIZE)) {
						dropCol = (mousePos.x - BORDER_SIZE) / SQUARE_SIZE;
						dropRow = (mousePos.y - BORDER_SIZE) / SQUARE_SIZE;
					} else {
						dropCol = -1;
						dropRow = -1;
					}
					if (dropRow >= 0 && dropCol >= 0)
						dropIndex = squareToIndex(7 - dropRow, dropCol);
					else {
						dropIndex = -1;
					}

					//std::cout << "Drop: " << dropCol << " " << dropRow << " Index: " << dropIndex << std::endl;

					if(index > -1) {
						if(board.board[index] != NULL) {
							boardGUI.selectPiece(board, index);
						} else {
							boardGUI.renderBoardWithPieces(board);
						}
					}
				}
			}

			if(e.type == SDL_MOUSEBUTTONDOWN) {
				if (!leftMouseButtonDown && e.button.button == SDL_BUTTON_LEFT) {
					leftMouseButtonDown = true;
					if(mousePos.x < (WIN_WIDTH - BORDER_SIZE) && (mousePos.x > BORDER_SIZE) && mousePos.y < (WIN_HEIGHT - BORDER_SIZE) && (mousePos.y > BORDER_SIZE)) {
						focusCol = (mousePos.x - BORDER_SIZE) / SQUARE_SIZE;
						focusRow = (mousePos.y - BORDER_SIZE) / SQUARE_SIZE;
					} else {
						focusCol = -1;
						focusRow = -1;
					}
					if (focusRow >= 0 && focusCol >= 0)
						index = squareToIndex((ROW - 1) - focusRow, focusCol);
					else {
						index = -1;
					}
					std::cout << "Focus: " << focusCol << " " << focusRow << " Index: " << index << std::endl;

					if(index > -1) {
						if(board.board[index] != NULL) {
							boardGUI.selectPiece(board, index);
						} else {
							boardGUI.renderBoardWithPieces(board);
						}
					}

				}
			}
			
		}

	}

	if(executeOnce) {
		executeOnce = false;
		SDL_Delay(1000);
		board.movePiece(28, 35);
		boardGUI.renderBoardWithPieces(board);
		SDL_Delay(1000);
		board.board[42]->computeValidMoves(board);
		board.movePiece(42, 35);
		boardGUI.renderBoardWithPieces(board);
		SDL_Delay(3000);
	}

	return 0;

}