#include "Graphics.hpp"

/* Defined Headers */
#include "Board.hpp"
#include "King.hpp"
#include "SDL_mixer.h"
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
const int BORDER_SIZE = 50; /* Suggested: 45 */
const int WIN_WIDTH = COL * SQUARE_SIZE + 2 * BORDER_SIZE;
const int WIN_HEIGHT = ROW * SQUARE_SIZE + 2 * BORDER_SIZE;
int physW, physH; /* Logical Size */
float scaleX, scaleY; /* Scaling Factor */

/* ##### Board Color Properties ##### */
const SDL_Color WHITE_SQUARE = {0xEC, 0xDA, 0xB9, 0xFF};
const SDL_Color BLACK_SQUARE = {0xAE, 0x8A, 0x68, 0xFF};
const SDL_Color BKGD_COLOR = {0x16, 0x15, 0x12, 0xFF};
const SDL_Color HIGHLIGHT = {0x7F, 0x17, 0x1F, 0x80};
const SDL_Color BOARD_TEXT = {0xFF, 0xFF, 0xFF, 0xFF};

/* Move Animations */
const int durationMs = 100;
const int fps = 120;

/* ##### Global Textures ##### */
Texture whitePieces[7];
Texture blackPieces[7];
Texture moveDot;
Texture kingInCheck;
Texture capture;
Texture hoverSquare;
Texture boardLetters[8];
Texture boardNumbers[8];

/* ##### Sound Effects ##### */
Mix_Chunk * gameStartSound = nullptr;
Mix_Chunk * gameEndSound = nullptr;
Mix_Chunk * captureSound = nullptr;
Mix_Chunk * castleSound = nullptr;
Mix_Chunk * moveSound = nullptr;
Mix_Chunk * moveCheckSound = nullptr;
Mix_Chunk * promoteSound = nullptr;
Mix_Chunk * illegalMoveSound = nullptr;
/* From: chess.com - https://www.chess.com/forum/view/general/chessboard-sound-files?page=2 */

/* Fonts */
const int baseFontSize = 24;
TTF_Font * boardFont;

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
    SDL_GL_GetDrawableSize(window, &physW, &physH);
    scaleX = physW / static_cast<float>(WIN_WIDTH); // logical width
    scaleY = physH / static_cast <float> (WIN_HEIGHT);  // logical height

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

    /* Initialize SDL_Mixer */
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        // Throw Error
    }

    /* Initialize SDL_ttf */
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
    }

    /* Initialize the Board Squares */
    for (int row = 0; row < ROW; ++row) {
        for (int col = 0; col < COL; ++col) {
            int index = row * COL + col;

            // SDL y-axis is top-down, so flip row to draw bottom-up
            int sdlRow = (ROW - 1) - row;

            squares[index] = {BORDER_SIZE - 1 + col * SQUARE_SIZE, BORDER_SIZE - 1 + sdlRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
        }
    }
    
}

Graphics::~Graphics() {
	//Destroy window	
	if (window != NULL)
	    SDL_DestroyWindow(window);
    if (renderer != NULL)    
        SDL_DestroyRenderer(renderer);

    /* Free Sound Effects */
    Mix_FreeChunk(gameStartSound);
    Mix_FreeChunk(gameEndSound);
    Mix_FreeChunk(captureSound);
    Mix_FreeChunk(castleSound);
    Mix_FreeChunk(moveSound);
    Mix_FreeChunk(moveCheckSound);
    Mix_FreeChunk(promoteSound);
    Mix_FreeChunk(illegalMoveSound);


    /* Set Sound Effects to nullptr */
    gameStartSound = nullptr;
    gameEndSound = nullptr;
    captureSound = nullptr;
    castleSound = nullptr;
    moveSound = nullptr;
    moveCheckSound = nullptr;
    promoteSound = nullptr;
    illegalMoveSound = nullptr;

	/* Quit SDL subsystems */
    instantiated = false;
	IMG_Quit();
	SDL_Quit();
    Mix_Quit();
    TTF_Quit();
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

    /* ##### LOAD SOUND EFFECTS */
    gameStartSound = Mix_LoadWAV("../assets/sounds/game-start.wav");
    if(gameStartSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    gameEndSound = Mix_LoadWAV("../assets/sounds/game-end.wav");
    if(gameEndSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    captureSound = Mix_LoadWAV("../assets/sounds/capture.wav");
    if(captureSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    castleSound = Mix_LoadWAV("../assets/sounds/castle.wav");
    if(castleSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    moveSound = Mix_LoadWAV("../assets/sounds/move.wav");
    if(moveSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    moveCheckSound = Mix_LoadWAV("../assets/sounds/move-check.wav");
    if(moveCheckSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    promoteSound = Mix_LoadWAV("../assets/sounds/promote.wav");
    if(promoteSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    illegalMoveSound = Mix_LoadWAV("../assets/sounds/illegal.wav");
    if(illegalMoveSound == nullptr) {
        std::cerr << "Failure to load sound effect! " << Mix_GetError() << std::endl;
        success = false;
    }

    /* Load Necessary Fonts */
    int scaledFontSize = baseFontSize * scaleY;
    boardFont = TTF_OpenFont("../assets/fonts/Fira_Sans/FiraSans-Medium.ttf", scaledFontSize);
    
    if (boardFont == nullptr) {
        std::cerr << "Failure to load boardFont! SDL_ttf Error: " << TTF_GetError() << std::endl;
        success = false;
    }

    /* Create the Board Letters and Numbers */
    for (int i = 0; i < ROW; ++i) {
        char fileChar = 'a' + i;
        std::string fileStr(1, fileChar);
        boardLetters[i].loadFromRenderedText(renderer, boardFont, fileStr, BOARD_TEXT);
    }

    for (int i = 0; i < ROW; ++i) {
        char fileChar = '1' + i;
        std::string ranksStr(1, fileChar);
        boardNumbers[i].loadFromRenderedText(renderer, boardFont, ranksStr, BOARD_TEXT);
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

    int index = squareToIndex(row, col);
    const SDL_Rect& fillRect = squares[index];

    if ((row + col) % 2 != 0) {
        SDL_SetRenderDrawColor(renderer, WHITE_SQUARE.r, WHITE_SQUARE.g, WHITE_SQUARE.b, WHITE_SQUARE.a);
    } else {
        SDL_SetRenderDrawColor(renderer, BLACK_SQUARE.r, BLACK_SQUARE.g, BLACK_SQUARE.b, BLACK_SQUARE.a);
    }

    SDL_RenderFillRect(renderer, &fillRect);
}

void Graphics::renderMarkings() {
    /* File Markings */
    for (int i = 0; i < COL; ++i) {
        boardLetters[i].renderText(renderer, BORDER_SIZE - 1 + (SQUARE_SIZE * i) + (SQUARE_SIZE/2 - boardLetters[i].getWidth()/(2*scaleX)), WIN_HEIGHT - BORDER_SIZE + boardLetters[5].getWidth()/(2*scaleX), scaleY);
    }
    /* Row Markings */
    for (int i = 0; i < ROW; ++i) {
        boardNumbers[ROW - 1 - i].renderText(renderer, BORDER_SIZE/2 - boardNumbers[i].getWidth()/(2*scaleX), BORDER_SIZE - 1 + (SQUARE_SIZE * i) + (SQUARE_SIZE/2 - boardNumbers[i].getHeight()/(2*scaleY)), scaleY);
    }
}

void Graphics::renderBoard() {
    SDL_SetRenderDrawColor(renderer, BKGD_COLOR.r, BKGD_COLOR.g, BKGD_COLOR.b, BKGD_COLOR.a);
    SDL_RenderClear(renderer);

    for (int col = 0; col < COL; ++col) {
		for (int row = 0; row < ROW; ++row) {
			renderBoardSquare(col, row);
		}
        
	}
    renderMarkings();
}

void Graphics::renderPiece(const Board & board, int index) {
    
    if(index < 0 && index > (COL*ROW - 1))
        return;
    
    Color color;
	PieceType piece;

    if(board.board[index] != NULL) {
        color = board.board[index]->getColor();
        piece = board.board[index]->getType();
        int pieceID = static_cast<int>(piece);

        const SDL_Rect& dstRect = squares[index];

        if(piece == PieceType::King) {
            King * king = static_cast<King *>(board.board[index]);
            if (king->getCheckStatus()) {
                kingInCheck.renderTexture(renderer, dstRect.x, dstRect.y);
            }
        }
        
        if (color == Color::White)
            whitePieces[pieceID].renderTexture(renderer, dstRect.x, dstRect.y);
        
        if (color == Color::Black)
            blackPieces[pieceID].renderTexture(renderer, dstRect.x, dstRect.y);
    }
}

void Graphics::renderKingInCheck(int index) {
    const SDL_Rect dstRect = squares[index];
    kingInCheck.renderTexture(renderer, dstRect.x, dstRect.y);
    
}

void Graphics::renderPieces(const Board & board) {  
    for (int i = 0; i < COL * ROW; i++) {
		renderPiece(board, i);
	}
}

void Graphics::highlightSquare(int index) {
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, HIGHLIGHT.r, HIGHLIGHT.g, HIGHLIGHT.b, HIGHLIGHT.a);
	SDL_RenderFillRect(renderer, &squares[index]);
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void Graphics::highlightMove(int index) {
    const SDL_Rect dstRect = squares[index];
    moveDot.renderTexture(renderer, dstRect.x, dstRect.y);
}

void Graphics::highlightCapture(int index) {
    const SDL_Rect dstRect = squares[index];
    capture.renderTexture(renderer, dstRect.x, dstRect.y);
}

void Graphics::highlightPossibleMoves(const Board & board, int index) {
    if (index > 63 || index < 0) return;
    if (board.board[index] == nullptr) return;
    Piece * piece = board.board[index];

    for (int possibleMove : piece->validMoves) {
        if (board.board[possibleMove] != nullptr) {
             highlightCapture(possibleMove);
        } else {
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
    int fromRow = indexToRow(fromIndex);

    int startX = squares[fromIndex].x;
    int startY = squares[fromIndex].y;
    int endX = squares[toIndex].x;
    int endY = squares[toIndex].y;

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
