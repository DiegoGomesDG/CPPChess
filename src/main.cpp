#include "../include/Graphics.hpp"
#include "../include/Board.hpp"
#include "../include/Texture.hpp"

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
		fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w";
	}

	board.loadFromFEN(fen);
	boardGUI.renderPieces(board);

	//Update screen
	boardGUI.updateWindow();

	SDL_Event e;
	bool quit = false;
	bool leftMouseButtonDown = false;
	SDL_Point mousePos;
	int focusRow, focusCol;
	int index;

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
					boardGUI.clearWindow();
						boardGUI.renderBoard();
						boardGUI.renderPieces(board);
						boardGUI.highlightSquare(focusCol, focusRow);
						boardGUI.dragPiece(board, index, mousePos.x, mousePos.y);
						boardGUI.updateWindow();
				}
			}

			if(e.type == SDL_MOUSEBUTTONUP) {
				if(leftMouseButtonDown && e.button.button == SDL_BUTTON_LEFT) {
					leftMouseButtonDown = false;

					if(index > -1) {
						if(board.board[index] != NULL) {
							boardGUI.clearWindow();
							boardGUI.renderBoard();

							boardGUI.highlightSquare(focusCol, focusRow);
							
							boardGUI.renderPieces(board);
							boardGUI.updateWindow();
						} else {
							boardGUI.clearWindow();
							boardGUI.renderBoard();
							boardGUI.renderPieces(board);
							boardGUI.updateWindow();
						}
					}
				}
			}

			if(e.type == SDL_MOUSEBUTTONDOWN) {
				if (!leftMouseButtonDown && e.button.button == SDL_BUTTON_LEFT) {
					leftMouseButtonDown = true;
					if(mousePos.x < (WIN_WIDTH + BORDER_SIZE) && (mousePos.x > BORDER_SIZE) && mousePos.y < (WIN_HEIGHT + BORDER_SIZE) && (mousePos.y > BORDER_SIZE)) {
						focusCol = (mousePos.x - BORDER_SIZE) / SQUARE_SIZE;
						focusRow = (mousePos.y - BORDER_SIZE) / SQUARE_SIZE;
					} else {
						focusCol = -1;
						focusRow = -1;
					}
					if (focusRow >= 0 && focusCol >= 0)
						index = squareToIndex(7 - focusRow, focusCol);
					else {
						index = -1;
					}
					std::cout << "Focus: " << focusCol << " " << focusRow << " Index: " << index << std::endl;

					if(index > -1) {
						if(board.board[index] != NULL) {
							boardGUI.clearWindow();
							boardGUI.renderBoard();

							boardGUI.highlightSquare(focusCol, focusRow);
							
							boardGUI.renderPieces(board);
							boardGUI.updateWindow();
						} else {
							boardGUI.clearWindow();
							boardGUI.renderBoard();
							boardGUI.renderPieces(board);
							boardGUI.updateWindow();
						}
					}

				}
			}
			
		}

	}

	return 0;

}