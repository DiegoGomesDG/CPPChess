#include "graphics.hpp"

/* Defined Headers */
#include "Board.hpp"
#include "Texture.hpp"
#include "Piece.hpp"

/* ##### Libraries ##### */
#include <iostream>
#include <cassert>

/* ##### Window properties according to the size of the board ##### */
extern const int ROW = 8;
extern const int COL = 8;
extern const int SQUARE_SIZE = 90; /* Suggested: 90 */
extern const int BORDER_SIZE = 45; /* Suggested: 45 */
extern const int WIN_WIDTH = COL * SQUARE_SIZE + 2 * BORDER_SIZE;
extern const int WIN_HEIGHT = ROW * SQUARE_SIZE + 2 * BORDER_SIZE;

/* ##### Board Color Properties ##### */
const SDL_Color WHITE_SQUARE = {0xEC, 0xDA, 0xB9, 0xFF};
const SDL_Color BLACK_SQUARE = {0xAE, 0x8A, 0x68, 0xFF};
const SDL_Color BKGD_COLOR = {0x16, 0x15, 0x12, 0xFF};
const SDL_Color HIGHLIGHT = {0x7F, 0x17, 0x1F, 0x80};

/* ##### Global Texture ##### */
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
    float scaleX = physW / static_cast<float>(WIN_WIDTH); // logical width
    float scaleY = physH / static_cast <float> (WIN_HEIGHT);  // logical height

    /* Create RENDERER for the Window*/
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

    if(!whitePieces[static_cast<int>(PieceType::Pawn)].loadTexture("../assets/white/WhitePawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Knight)].loadTexture("../assets/white/WhiteKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Bishop)].loadTexture("../assets/white/WhiteBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Rook)].loadTexture("../assets/white/WhiteRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Queen)].loadTexture("../assets/white/WhiteQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::King)].loadTexture("../assets/white/WhiteKing.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    
    /* ##### Black Pieces ##### */
    if(!blackPieces[static_cast<int>(PieceType::Pawn)].loadTexture("../assets/black/BlackPawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Knight)].loadTexture("../assets/black/BlackKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Bishop)].loadTexture("../assets/black/BlackBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Rook)].loadTexture("../assets/black/BlackRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Queen)].loadTexture("../assets/black/BlackQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::King)].loadTexture("../assets/black/BlackKing.png", renderer)) {
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

void Graphics::clearWindow() {
    SDL_SetRenderDrawColor(renderer, BKGD_COLOR.r, BKGD_COLOR.g, BKGD_COLOR.b, BKGD_COLOR.a);
    SDL_RenderClear(renderer);
}

void Graphics::updateWindow() {
    SDL_RenderPresent(renderer);
}

void Graphics::renderBoard() {
    SDL_SetRenderDrawColor(renderer, BKGD_COLOR.r, BKGD_COLOR.g, BKGD_COLOR.b, BKGD_COLOR.a);
    SDL_RenderClear(renderer);

    for (int column = 0; column < COL; ++column) {
		for (int row = 0; row < ROW; ++row) {
			SDL_Rect fillRect = {SQUARE_SIZE * column + BORDER_SIZE - 1, SQUARE_SIZE * row + BORDER_SIZE - 1, SQUARE_SIZE, SQUARE_SIZE};
			if ((row + column) % 2 == 0)
				SDL_SetRenderDrawColor(renderer, WHITE_SQUARE.r, WHITE_SQUARE.g, WHITE_SQUARE.b, WHITE_SQUARE.a);
			else
				SDL_SetRenderDrawColor(renderer, BLACK_SQUARE.r, BLACK_SQUARE.g, BLACK_SQUARE.b, BLACK_SQUARE.a);
			
			SDL_RenderFillRect(renderer, &fillRect);

		}
	}
}

void Graphics::renderPiece(const Board & board, int index) {
    
    if(index < 0 && index > (COL*ROW - 1))
        return;
    
    Color color;
	PieceType piece;
	int row;
	int column;

    if(board.board[index] != NULL) {
        color = board.board[index]->getColor();
        piece = board.board[index]->getType();
        row = board.board[index]->getRow();
        column = board.board[index]->getColumn();
        int pieceID = static_cast<int>(piece);

        row = (ROW - 1) - row; /* Invert */
        
        if (color == Color::White)
            whitePieces[pieceID].renderTexture(renderer, BORDER_SIZE - 1 + SQUARE_SIZE * column, BORDER_SIZE - 1 + SQUARE_SIZE * row);
        
        if (color == Color::Black)
            blackPieces[pieceID].renderTexture(renderer, BORDER_SIZE - 1 + SQUARE_SIZE * column, BORDER_SIZE - 1 + SQUARE_SIZE * row);
    }
}

void Graphics::highlightSquare(int col, int row) {
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, HIGHLIGHT.r, HIGHLIGHT.g, HIGHLIGHT.b, HIGHLIGHT.a);
	SDL_Rect highlRect = {SQUARE_SIZE * col + BORDER_SIZE - 1, SQUARE_SIZE * row + BORDER_SIZE - 1, SQUARE_SIZE, SQUARE_SIZE};
	SDL_RenderFillRect(renderer, &highlRect);
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Graphics::renderPieces(const Board & board) {
    
    for (int i = 0; i < COL * ROW; i++) {
		renderPiece(board, i);
	    }
}

/* https://gigi.nullneuron.net/gigilabs/sdl2-drag-and-drop/?fbclid=IwY2xjawJcp9dleHRuA2FlbQIxMAABHVwbudUFVEK3WEu3RsJArnH2_GUbucPv5NFXbvub048pgzXka-PFcwqrIg_aem_hUfSUIs5p6Sm0uZ4gBEfHg */
void Graphics::dragPiece(const Board & board, int index, int mouseX, int mouseY) {

    Color color;
	PieceType piece;
    color = board.board[index]->getColor();
	piece = board.board[index]->getType();
    int pieceID = static_cast<int>(piece);

    if (color == Color::White)
		whitePieces[pieceID].renderTexture(renderer, mouseX - SQUARE_SIZE/2, mouseY - SQUARE_SIZE/2);
			
	if (color == Color::Black)
		blackPieces[pieceID].renderTexture(renderer, mouseX - SQUARE_SIZE/2, mouseY - SQUARE_SIZE/2);
}