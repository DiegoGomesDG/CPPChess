#include "Graphics.hpp"

/* Defined Headers */
#include "Board.hpp"
#include "Texture.hpp"
#include "Piece.hpp"
#include "Pawn.hpp"

/* ##### Libraries ##### */
#include <iostream>
#include <cassert>

/* ##### Window properties according to the size of the board ##### */
const int ROW = 8;
const int COL = 8;
const int SQUARE_SIZE = 90; /* Suggested: 90 */
const int BORDER_SIZE = 45; /* Suggested: 45 */
const int WIN_WIDTH = COL * SQUARE_SIZE + 2 * BORDER_SIZE;
const int WIN_HEIGHT = ROW * SQUARE_SIZE + 2 * BORDER_SIZE;

/* ##### Board Color Properties ##### */
const SDL_Color WHITE_SQUARE = {0xEC, 0xDA, 0xB9, 0xFF};
const SDL_Color BLACK_SQUARE = {0xAE, 0x8A, 0x68, 0xFF};
const SDL_Color BKGD_COLOR = {0x16, 0x15, 0x12, 0xFF};
const SDL_Color HIGHLIGHT = {0x7F, 0x17, 0x1F, 0x80};

/* Move Animations */
const int durationMs = 100;
const int fps = 120;

/* ##### Global Texture ##### */
Texture whitePieces[7];
Texture blackPieces[7];
Texture moveDot;
Texture kingInCheck;
Texture capture;
Texture hoverSquare;

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

    /* ##### Square that appears when the mouse is hovering */
    if(!hoverSquare.loadTexture("../assets/HoverSquare.png", renderer)) {
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

void Graphics::renderBoardSquare(int col, int row) {
    if (col < 0 || row < 0 || col >= COL || row >= ROW) return;   

    SDL_Rect fillRect = {SQUARE_SIZE * col + BORDER_SIZE - 1, SQUARE_SIZE * row + BORDER_SIZE - 1, SQUARE_SIZE, SQUARE_SIZE};

    if ((row + col) % 2 == 0) {
        SDL_SetRenderDrawColor(renderer, WHITE_SQUARE.r, WHITE_SQUARE.g, WHITE_SQUARE.b, WHITE_SQUARE.a);
    } else {
        SDL_SetRenderDrawColor(renderer, BLACK_SQUARE.r, BLACK_SQUARE.g, BLACK_SQUARE.b, BLACK_SQUARE.a);
    }
			
	SDL_RenderFillRect(renderer, &fillRect);
}

void Graphics::renderBoard() {
    SDL_SetRenderDrawColor(renderer, BKGD_COLOR.r, BKGD_COLOR.g, BKGD_COLOR.b, BKGD_COLOR.a);
    SDL_RenderClear(renderer);

    for (int col = 0; col < COL; ++col) {
		for (int row = 0; row < ROW; ++row) {
			renderBoardSquare(col, row);

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

void Graphics::renderPieces(const Board & board) {  
    for (int i = 0; i < COL * ROW; i++) {
		renderPiece(board, i);
	}
}

void Graphics::highlightSquare(int index) {
    int col = indexToColumn(index);
    int row = (ROW - 1) - indexToRow(index); /* Invert */
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, HIGHLIGHT.r, HIGHLIGHT.g, HIGHLIGHT.b, HIGHLIGHT.a);
	SDL_Rect highlRect = {SQUARE_SIZE * col + BORDER_SIZE - 1, SQUARE_SIZE * row + BORDER_SIZE - 1, SQUARE_SIZE, SQUARE_SIZE};
	SDL_RenderFillRect(renderer, &highlRect);
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Graphics::highlightMove(int index) {
    int col = indexToColumn(index);
    int row = (ROW - 1) - indexToRow(index); /* Invert */
    moveDot.renderTexture(renderer, BORDER_SIZE - 1 + SQUARE_SIZE * col, BORDER_SIZE - 1 + SQUARE_SIZE * row);
}

void Graphics::highlightCapture(int index) {
    int col = indexToColumn(index);
    int row = (ROW - 1) - indexToRow(index); /* Invert */
    capture.renderTexture(renderer, BORDER_SIZE - 1 + SQUARE_SIZE * col, BORDER_SIZE - 1 + SQUARE_SIZE * row);
}

void Graphics::highlightPossibleMoves(const Board & board, int index) {
    if (index > 63 || index < 0) return;
    if (board.board[index] == nullptr) return;
    Piece * piece = board.board[index];

    for (int possibleMove : piece->validMoves) {
        if (board.board[possibleMove] != nullptr) {
             highlightCapture(possibleMove);
        } else {
            if(piece->getType() != PieceType::Pawn) highlightMove(possibleMove);
        }
    }
    
    /* Exception to Pawns */
    if (piece->getType() == PieceType::Pawn) {
        Pawn * pawn = static_cast<Pawn *>(piece);
        for (int possibleMove : pawn->forwardMoves) {
            highlightMove(possibleMove);
        }
    }
}

void Graphics::selectPiece(const Board & board, int index) {
    clearWindow();
	renderBoard();
	highlightSquare(index);
	renderPieces(board);
    highlightPossibleMoves(board, index);
	updateWindow();
}

void Graphics::renderBoardWithPieces(const Board & board) {
    clearWindow();
    renderBoard();
	renderPieces(board);
	updateWindow();
}

void Graphics::renderHoverSquare(int mouseX, int mouseY) {
    int hoverCol, hoverRow;
    
    if(mouseX < (WIN_WIDTH - BORDER_SIZE) && (mouseX > BORDER_SIZE) && mouseY < (WIN_HEIGHT - BORDER_SIZE) && (mouseY > BORDER_SIZE)) {
        hoverCol = (mouseX - BORDER_SIZE) / SQUARE_SIZE;
        hoverRow = (mouseY - BORDER_SIZE) / SQUARE_SIZE;
    } else {
        hoverCol = -1;
        hoverRow = -1;
    }

    if (hoverCol > -1 && hoverRow > -1)
        hoverSquare.renderTexture(renderer, BORDER_SIZE - 1 + SQUARE_SIZE * hoverCol, BORDER_SIZE - 1 + SQUARE_SIZE * hoverRow);
}

/* https://gigi.nullneuron.net/gigilabs/sdl2-drag-and-drop/?fbclid=IwY2xjawJcp9dleHRuA2FlbQIxMAABHVwbudUFVEK3WEu3RsJArnH2_GUbucPv5NFXbvub048pgzXka-PFcwqrIg_aem_hUfSUIs5p6Sm0uZ4gBEfHg */
void Graphics::renderDraggedPiece(const Board & board, int index, int mouseX, int mouseY) {

    Color color;
	PieceType piece;
    color = board.board[index]->getColor();
	piece = board.board[index]->getType();
    int pieceID = static_cast<int>(piece);

    clearWindow();
	renderBoard();
    renderHoverSquare(mouseX, mouseY);
	renderPieces(board);
    highlightSquare(index);
    highlightPossibleMoves(board, index);

    if (color == Color::White)
		whitePieces[pieceID].renderTexture(renderer, mouseX - SQUARE_SIZE/2, mouseY - SQUARE_SIZE/2);
			
	if (color == Color::Black)
		blackPieces[pieceID].renderTexture(renderer, mouseX - SQUARE_SIZE/2, mouseY - SQUARE_SIZE/2);

    updateWindow();
}

void Graphics::animatePieceMoving(const Board & board, int fromIndex, int toIndex) {
    int fromCol = indexToColumn(fromIndex);
    int fromRow = (ROW - 1) - indexToRow(fromIndex);
    int toCol = indexToColumn(toIndex);
    int toRow = (ROW - 1) - indexToRow(toIndex);

    int startX = BORDER_SIZE - 1 + fromCol * SQUARE_SIZE;
    int startY = BORDER_SIZE - 1 + fromRow * SQUARE_SIZE;
    int endX = BORDER_SIZE - 1 + toCol * SQUARE_SIZE;
    int endY = BORDER_SIZE - 1 + toRow * SQUARE_SIZE;

    Piece* movingPiece = board.board[fromIndex];
    if (!movingPiece) return;

    const int frameDelay = 1000 / fps;
    const int totalFrames = durationMs / frameDelay;

    for (int frame = 0; frame <= totalFrames; ++frame) {
        float t = static_cast<float>(frame) / totalFrames;
        int currentX = static_cast<int>(startX + (endX - startX) * t);
        int currentY = static_cast<int>(startY + (endY - startY) * t);

        // Clear screen and render base board
        clearWindow();
        renderBoard();

        // Draw all other pieces
        renderPieces(board);

        // Redraw the square where the moving piece was, to "hide" it
        renderBoardSquare(fromCol, fromRow);

        // Draw the moving piece at its interpolated position
        if (movingPiece->getColor() == Color::White)
            whitePieces[static_cast<int>(movingPiece->getType())].renderTexture(renderer, currentX, currentY);
        else
            blackPieces[static_cast<int>(movingPiece->getType())].renderTexture(renderer, currentX, currentY);

        updateWindow();
        SDL_Delay(frameDelay);
    }
}
