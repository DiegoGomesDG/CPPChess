#include "graphics.hpp"

/* ##### Libraries ##### */
#include <iostream>
#include <cassert>

/* ##### Window properties according to the size of the board ##### */
extern const int SQUARE_SIZE = 90;
extern const int BORDER_SIZE = 45;
extern const int WIN_WIDTH = 8 * SQUARE_SIZE + 2 * BORDER_SIZE;
extern const int WIN_HEIGHT = 8 * SQUARE_SIZE + 2 * BORDER_SIZE;

/* ##### Board Properties ##### */
const SDL_Color WHITE_SQUARE = {0xEC, 0xDA, 0xB9, 0xFF};
const SDL_Color BLACK_SQUARE = {0xAE, 0x8A, 0x68, 0xFF};
const SDL_Color BKGD_COLOR = {0x16, 0x15, 0x12, 0xFF};

/* ##### Texture ##### */
Texture whitePieces[7];
Texture blackPieces[7];
Texture moveDot;
Texture kingInCheck;
Texture capture;

/* ##### Static Variables ##### */
bool Graphics::instantiated = false;

Graphics::Graphics() {
    
    assert(!instantiated && "More than one instance of the Class Graphics is not allowed!");
    instantiated = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError(); // Implement error handling
        // Throw Error
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        std::cerr << "Warning: Linear texture filtering not enabled!";

    /* Create WINDOW */
    window = SDL_CreateWindow("CPPChess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    
    if (window == NULL) {
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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
    if(renderer == NULL) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError();
        SDL_Quit();
        // Throw Error
    }
    SDL_RenderSetScale(renderer, scaleX, scaleY);

    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    /* Initialize PNG Loading */
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "DL_image could not initialize! SDL_image Error: " << SDL_GetError();
        // Throw Error
    }
}

Graphics::~Graphics() {
	//Destroy window	
	if (window != NULL)
	    SDL_DestroyWindow(window);
    if (renderer != NULL)    
        SDL_DestroyRenderer(renderer);

    instantiated = false;
	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

/* Source of the Files: https://commons.wikimedia.org/wiki/Category:SVG_chess_pieces */
bool Graphics::loadMedia() {
    bool success = true;
    
    /* ##### White Pieces ##### */
    /* loadTexture returns true if it is successful, false otherwise. So if it is not true, it means it failed*/

    if(!whitePieces[(int) PieceType::Pawn].loadTexture("../assets/white/WhitePawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[(int) PieceType::Knight].loadTexture("../assets/white/WhiteKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[(int) PieceType::Bishop].loadTexture("../assets/white/WhiteBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[(int) PieceType::Rook].loadTexture("../assets/white/WhiteRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[(int) PieceType::Queen].loadTexture("../assets/white/WhiteQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[(int) PieceType::King].loadTexture("../assets/white/WhiteKing.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    
    /* ##### Black Pieces ##### */
    if(!blackPieces[(int) PieceType::Pawn].loadTexture("../assets/black/BlackPawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[(int) PieceType::Knight].loadTexture("../assets/black/BlackKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[(int) PieceType::Bishop].loadTexture("../assets/black/BlackBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[(int) PieceType::Rook].loadTexture("../assets/black/BlackRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[(int) PieceType::Queen].loadTexture("../assets/black/BlackQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[(int) PieceType::King].loadTexture("../assets/black/BlackKing.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }

    /* ##### Move Dot ##### */
    if(!moveDot.loadTexture("../assets/MoveDot.png", renderer)) {
        std::cerr << "Failed to load texture!";
        success = false;
    }

    /* ##### King in Check Highlight ##### */
    if(!kingInCheck.loadTexture("../assets/KingInCheck.png", renderer)) {
        std::cerr << "Failed to load texture!";
        success = false;
    }

    /* ##### Capture a Piece Highlight ##### */
    if(!capture.loadTexture("../assets/Capture.png", renderer)) {
        std::cerr << "Failed to load texture!";
        success = false;
    }

    return success;
}

SDL_Window * Graphics::getWindow() {
    return window;
}

SDL_Renderer * Graphics::getRenderer() {
    return renderer;
}

