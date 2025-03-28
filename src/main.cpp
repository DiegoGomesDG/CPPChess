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

int main(int argc, char* args[]) {
	//Start up SDL and create window
	Graphics board;
	board.loadMedia();

	//Clear screen
	SDL_SetRenderDrawColor( board.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF );
	SDL_RenderClear( board.getRenderer() );
	
	// Draw Brown Background
	SDL_SetRenderDrawColor( board.getRenderer(), 0x16, 0x15, 0x12, 0xFF );
	SDL_Rect bkgdRect = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	SDL_RenderFillRect(board.getRenderer(), &bkgdRect);
	
	// Draw Chess board
	SDL_SetRenderDrawColor( board.getRenderer(), 0xFF, 0xFF, 0xFF, 0xFF );
	for (int column = 0; column < 8; ++column) {
		for (int rank = 0; rank < 8; ++rank) {
			SDL_Rect fillRect = {SQUARE_SIZE * column + BORDER_SIZE -1, SQUARE_SIZE * rank + BORDER_SIZE -1, SQUARE_SIZE, SQUARE_SIZE};
			if ((rank+column) % 2 == 0)
				SDL_SetRenderDrawColor( board.getRenderer(), 0xEC, 0xDA, 0xB9, 0xFF );
			else
				SDL_SetRenderDrawColor( board.getRenderer(), 0xAE, 0x8A, 0x68, 0xFF );
			
			SDL_RenderFillRect( board.getRenderer(), &fillRect);
			
			SDL_SetRenderDrawColor(board.getRenderer(), 0xFF, 0x00, 0x00, 0xFF );
			
		}
	}
	
	for (int i = 0; i < 8; ++i) {
		whitePieces[1].renderTexture(board.getRenderer(), 29 + 90 * i, 29 + 90 * 6);
		blackPieces[1].renderTexture(board.getRenderer(), 29 + 90 * i, 29 + 90 * 1);
	}

	whitePieces[6].renderTexture(board.getRenderer(), 29 + 90 * 4, 29 + 90 * 7);
	whitePieces[5].renderTexture(board.getRenderer(), 29 + 90 * 3, 29 + 90 * 7);
	whitePieces[2].renderTexture(board.getRenderer(), 29 + 90 * 1, 29 + 90 * 7);
	whitePieces[2].renderTexture(board.getRenderer(), 29 + 90 * 6, 29 + 90 * 7);
	whitePieces[3].renderTexture(board.getRenderer(), 29 + 90 * 2, 29 + 90 * 7);
	whitePieces[3].renderTexture(board.getRenderer(), 29 + 90 * 5, 29 + 90 * 7);
	whitePieces[4].renderTexture(board.getRenderer(), 29 + 90 * 0, 29 + 90 * 7);
	whitePieces[4].renderTexture(board.getRenderer(), 29 + 90 * 7, 29 + 90 * 7);
	
	blackPieces[6].renderTexture(board.getRenderer(), 29 + 90 * 4, 29 + 90 * 0);
	blackPieces[5].renderTexture(board.getRenderer(), 29 + 90 * 3, 29 + 90 * 0);
	blackPieces[2].renderTexture(board.getRenderer(), 29 + 90 * 1, 29 + 90 * 0);
	blackPieces[2].renderTexture(board.getRenderer(), 29 + 90 * 6, 29 + 90 * 0);
	blackPieces[3].renderTexture(board.getRenderer(), 29 + 90 * 2, 29 + 90 * 0);
	blackPieces[3].renderTexture(board.getRenderer(), 29 + 90 * 5, 29 + 90 * 0);
	blackPieces[4].renderTexture(board.getRenderer(), 29 + 90 * 0, 29 + 90 * 0);
	blackPieces[4].renderTexture(board.getRenderer(), 29 + 90 * 7, 29 + 90 * 0);
	

	//Update screen
	SDL_RenderPresent(board.getRenderer());

	SDL_Event e;
	bool quit = false;
	while(!quit) {
			//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 ) {
			//User requests quit
			if( e.type == SDL_QUIT ) {
				quit = true;
			}
		}
	}

	return 0;
}