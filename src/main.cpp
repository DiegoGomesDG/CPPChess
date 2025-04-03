#include "../include/graphics.hpp"
#include "../include/board.hpp"
#include "../include/texture.hpp"

extern int SQUARE_SIZE;
extern int BORDER_SIZE;
extern int WIN_WIDTH;
extern int WIN_HEIGHT;


//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL2/SDL.h>

extern Texture whitePieces[7];
extern Texture moveDot;
extern Texture kingInCheck;
extern Texture capture;

int main(int argc, char* argv[]) {
	//Start up SDL and create window
	Graphics boardGUI;
	boardGUI.loadMedia();

	//Clear screen
	SDL_SetRenderDrawColor( boardGUI.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( boardGUI.getRenderer() );
	
	// Draw Brown Background
	SDL_SetRenderDrawColor( boardGUI.getRenderer(), 0x16, 0x15, 0x12, 0xFF );
	SDL_Rect bkgdRect = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	SDL_RenderFillRect(boardGUI.getRenderer(), &bkgdRect);
	
	// Draw Chess board
	SDL_SetRenderDrawColor( boardGUI.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF );
	for (int column = 0; column < 8; ++column) {
		for (int rank = 0; rank < 8; ++rank) {
			SDL_Rect fillRect = {SQUARE_SIZE * column + BORDER_SIZE -1, SQUARE_SIZE * rank + BORDER_SIZE -1, SQUARE_SIZE, SQUARE_SIZE};
			if ((rank+column) % 2 == 0)
				SDL_SetRenderDrawColor(boardGUI.getRenderer(), 0xEC, 0xDA, 0xB9, 0xFF);
			else
				SDL_SetRenderDrawColor(boardGUI.getRenderer(), 0xAE, 0x8A, 0x68, 0xFF);
			
			SDL_RenderFillRect( boardGUI.getRenderer(), &fillRect);
			
			SDL_SetRenderDrawColor(boardGUI.getRenderer(), 0xFF, 0x00, 0x00, 0xFF );
			
			
		}
	}
	
	Board board;
	std::string fen;
	if (argc > 1) {
		 fen = argv[1];
	} else {
		fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	}

	board.loadFromFEN(fen);

	for (int i = 0; i < 64; i++) {
		Color color;
		PieceType piece;
		int row;
		int column;

		if(board.board[i] != NULL) {
			color = board.board[i]->getColor();
			piece = board.board[i]->getType();
			row = indexToRow(board.board[i]->getPosition());
			column = indexToColumn(board.board[i]->getPosition());
			int pieceID = static_cast<int>(piece);
			std::cout << "Index:" << i << "   Piece ID: " << pieceID << ". Color: " << (int)color << " Row: " << row << " Col: " << column << std::endl;

			row = 7 - row;
			
			if (color == Color::White) {
				whitePieces[pieceID].renderTexture(boardGUI.getRenderer(), BORDER_SIZE - 1 + 90 * column, BORDER_SIZE - 1 + 90 * row);
			}
			if (color == Color::Black) {
				blackPieces[pieceID].renderTexture(boardGUI.getRenderer(), BORDER_SIZE - 1 + 90 * column, BORDER_SIZE - 1 + 90 * row);
			}

	}

	}

	//Update screen
	SDL_RenderPresent(boardGUI.getRenderer());

	SDL_Event e;
	bool quit = false;
	while(!quit) {
			//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			int x, y;
			int row, column;
			if( e.type == SDL_QUIT ) {
				quit = true;
			}

			if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONUP) {
				
        		SDL_GetMouseState(&x, &y);
				
				if ((x > BORDER_SIZE - 1) && (x < WIN_WIDTH - BORDER_SIZE) && (y > BORDER_SIZE - 1) && (y < WIN_HEIGHT - BORDER_SIZE)) {
					row = (y - BORDER_SIZE) / SQUARE_SIZE;
					column = (x - BORDER_SIZE) / SQUARE_SIZE;
					
				} else {
					row = -1;
					column = -1;
				}
				// std::cout << "Row: " << row << std::endl;
				// std::cout << "Column: " << column << std::endl;
			}
			
		}
	}

	return 0;
}