
#include "../include/Game.hpp"

//Using SDL here for the SDL_Event
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
	
	std::string fen = (argc > 1) ? argv[1] : "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq";
    ChessGame game(fen);

    SDL_Event e;
    while (!game.isGameOver()) {
        while (SDL_PollEvent(&e)) {
            game.handleEvent(e);
        }
    }

	return 0;

}