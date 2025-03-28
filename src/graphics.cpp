#include "graphics.hpp"

/* ##### Libraries ##### */
#include <iostream>

/* ##### Window properties according to the size of the board ##### */
extern const int SQUARE_SIZE = 90;
extern const int BORDER_SIZE = 30;
extern const int WIN_WIDTH = 8 * SQUARE_SIZE + 2 * BORDER_SIZE;
extern const int WIN_HEIGHT = 8 * SQUARE_SIZE + 2 * BORDER_SIZE;

/* ##### Board Properties ##### */
const SDL_Color WHITE_SQUARE = {0xEC, 0xDA, 0xB9, 0xFF};
const SDL_Color BLACK_SQUARE = {0xAE, 0x8A, 0x68, 0xFF};
const SDL_Color BKGD_COLOR = {0xA4, 0x74, 0x49, 0xFF};

/* ##### Texture ##### */
Texture whitePieces[7];
Texture blackPieces[7];
Texture moveDot;
Texture kingInCheck;

Graphics::Graphics() {
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError(); // Implement error handling
        // Throw Error
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        std::cerr << "Warning: Linear texture filtering not enabled!";

    /* Create WINDOW */
    this->window = SDL_CreateWindow("CPPChess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    
    if (this->window == NULL) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError(); // Implement error handling
        SDL_Quit();
        // Throw Error
    }

    /* Get Retina Scaling Factors - HIGH DPI Macbook Screen - Scale of 2.0 */
    int physW, physH;
    SDL_GL_GetDrawableSize(window, &physW, &physH);
    float scaleX = physW / static_cast<float>(WIN_WIDTH); // 800 = logical width
    float scaleY = physH / static_cast <float> (WIN_HEIGHT);  // 600 = logical height

    /* Create RENDERER for the Window*/
    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
    if(this->renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError();
        SDL_Quit();
        // Throw Error
    }
    SDL_RenderSetScale(this->renderer, scaleX, scaleY);

    SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    /* Initialize PNG Loading */
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "DL_image could not initialize! SDL_image Error: " << SDL_GetError();
        // Throw Error
    }
}

Graphics::~Graphics() {
	//Destroy window	
	if (this->window != NULL)
	    SDL_DestroyWindow(this->window);
    if (this->renderer != NULL)    
        SDL_DestroyRenderer(this->renderer);

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

/*
Source of the Files:
https://commons.wikimedia.org/wiki/Category:SVG_chess_pieces
*/
bool Graphics::loadMedia() {
    bool success = true;
    SDL_Renderer * renderer = this->renderer;
    /* ##### White Pieces ##### */
    /* loadTexture returns true if it is successful, false otherwise. So if it is not true, it means it failed*/

    if(!whitePieces[Pawn].loadTexture("../assets/white/WhitePawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[Knight].loadTexture("../assets/white/WhiteKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[Bishop].loadTexture("../assets/white/WhiteBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[Rook].loadTexture("../assets/white/WhiteRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[Queen].loadTexture("../assets/white/WhiteQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[King].loadTexture("../assets/white/WhiteKing.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    
    /* ##### Black Pieces ##### */
    if(!blackPieces[Pawn].loadTexture("../assets/black/BlackPawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[Knight].loadTexture("../assets/black/BlackKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[Bishop].loadTexture("../assets/black/BlackBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[Rook].loadTexture("../assets/black/BlackRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[Queen].loadTexture("../assets/black/BlackQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[King].loadTexture("../assets/black/BlackKing.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }

    /* ##### Move Dot ##### */
/*     if(!moveDot.loadTexture("../assets/MoveDot.png", renderer)) {
        std::cerr << "Failed to load texture!";
        success = false;
    } */

    /* ##### King in Check Highlight ##### */
   /*  if(!moveDot.loadTexture("../assets/KingInCheck.png", renderer)) {
        std::cerr << "Failed to load texture!";
        success = false;
    } */

    return success;
}

SDL_Window * Graphics::getWindow() {
    return this->window;
}

SDL_Renderer * Graphics::getRenderer() {
    return this->renderer;
}

