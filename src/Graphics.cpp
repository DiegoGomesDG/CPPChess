#include "Graphics.hpp"

/* User-Defined Headers */
#include "Board.hpp"
#include "King.hpp"
#include "SDL_mixer.h"
#include "SDL_video.h"
#include "Texture.hpp"
#include "Piece.hpp"
#include "Pawn.hpp"
#include "ChessGUI.hpp"

/* ##### Standard Libraries ##### */
#include <iostream>
#include <cassert>
#include <stdexcept>

/* ##### Window properties according to the size of the board ##### */
const int ROW = 8;
const int COL = 8;
const int SQUARE_SIZE = 90; /* Suggested: 90 */
const int BORDER_SIZE = 50; /* Suggested: 45 */

const int LEFT_BORDER_SIZE = 50;
const int RIGHT_BORDER_SIZE = 50;
const int TOP_BORDER_SIZE = 50;
const int BOTTOM_BORDER_SIZE = 50;

const int WIN_WIDTH = COL * SQUARE_SIZE + LEFT_BORDER_SIZE + RIGHT_BORDER_SIZE;
const int WIN_HEIGHT = ROW * SQUARE_SIZE + TOP_BORDER_SIZE + BOTTOM_BORDER_SIZE;
int physW, physH; /* Logical Size */
float scaleX, scaleY; /* Scaling Factor */

/* ##### Board Color Properties ##### */
const SDL_Color DEFAULT_WHITE_SQUARE = {0xEC, 0xDA, 0xB9, 0xFF};
const SDL_Color DEFAULT_BLACK_SQUARE = {0xAE, 0x8A, 0x68, 0xFF};
const SDL_Color DEFAULT_BKGD_COLOR = {0x16, 0x15, 0x12, 0xFF};
SDL_Color WHITE_SQUARE;
SDL_Color BLACK_SQUARE;
SDL_Color BKGD_COLOR;
const SDL_Color HIGHLIGHT = {0x7F, 0x17, 0x1F, 0x80};
const SDL_Color BOARD_TEXT = {0xFF, 0xFF, 0xFF, 0xFF};
const SDL_Color STATUS_TEXT = {0xFF, 0xFF, 0xFF, 0xFF};

/* Move Animations */
int durationMs = 150;
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

/* Fonts */
const int boardMarkingsFontSize = 24;
const int statusFontSize = 56;

TTF_Font * boardFont;
TTF_Font * statusFont;

/* ##### Static Variables ##### */
//bool Graphics::instantiated = false;

/* Graphics Constructor. It initializes all the SDL Subsystems, taking care of the MacBook Pro 14" High DPI Display and also precomputes the squares of the board */
Graphics::Graphics() {
    
    //assert(!instantiated && "More than one instance of the Class Graphics is not allowed!");
    //instantiated = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError(); // Implement error handling
        throw std::runtime_error(std::string("SDL could not initialize! SDL Error: ") + SDL_GetError());
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        std::cerr << "Warning: Linear texture filtering not enabled!";

    /* ImGui Required*/
    // std::cerr << std::endl << SDL_GetCurrentVideoDriver();
    #ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
    #endif

    /* Create WINDOW */
    window = SDL_CreateWindow("CPPChess", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL Error: " << SDL_GetError(); // Implement error handling
        SDL_Quit();
        throw std::runtime_error(std::string("SDL Window could not be created! SDL Error: ") + SDL_GetError());
    }

    /* Get Retina Scaling Factors - HIGH DPI Macbook Screen - Scale of 2.0 */
    SDL_GL_GetDrawableSize(window, &physW, &physH);
    scaleX = physW / static_cast<float>(WIN_WIDTH); // logical width
    scaleY = physH / static_cast <float> (WIN_HEIGHT);  // logical height

    /* Create RENDERER for the Window*/
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_RenderSetIntegerScale(renderer, SDL_TRUE);
    if(renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError();
        SDL_Quit();
        throw std::runtime_error(std::string("SDL Renderer could not be created! SDL Error: ") + SDL_GetError());
    }
    SDL_RenderSetScale(renderer, scaleX, scaleY);
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

    /* Initialize PNG Loading */
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << SDL_GetError();
        throw std::runtime_error(std::string("SDL_Image could not initialize! SDL_Image Error: ") + SDL_GetError());
    }

    /* Initialize SDL_Mixer */
    if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        throw std::runtime_error(std::string("SDL_Mixer could not initialize! SDL_Mixer Error: ") + Mix_GetError());
    }

    /* Initialize SDL_TTF */
    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        throw std::runtime_error(std::string("SDL_TTF could not initialize! SDL_TTF Error: ") + TTF_GetError());
    }

    /* Initialize the Board Squares */
    for (int row = 0; row < ROW; ++row) {
        for (int col = 0; col < COL; ++col) {
            int index = row * COL + col;

            // SDL y-axis is top-down, so flip row to draw bottom-up
            int sdlRow = (ROW - 1) - row;

            squares[index] = {LEFT_BORDER_SIZE - 1 + col * SQUARE_SIZE, TOP_BORDER_SIZE - 1 + sdlRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
        }
    }

    /* Initialize Default Colors */
    WHITE_SQUARE = DEFAULT_WHITE_SQUARE;
    BLACK_SQUARE = DEFAULT_BLACK_SQUARE;
    BKGD_COLOR = DEFAULT_BKGD_COLOR;

    /* Board Flipped, Show Markings */
    showMarkings = true;
    isBoardFlipped = false;
    whiteAttack = false;
    blackAttack = false;

    /* Initialise ChessGUI pointer to null */ 
    gui = nullptr;
    
}

/* Graphics class destructor. It deallocates all SDL subsystem textures, chunks and windows, then quits the subsystems */
Graphics::~Graphics() {
	//Destroy window	
	if (window != nullptr)
	    SDL_DestroyWindow(window);
    if (renderer != nullptr)    
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

    /* Set the Pointer to null*/
    gui = nullptr;

	/* Quit SDL subsystems */
    //instantiated = false;
	IMG_Quit();
	SDL_Quit();
    Mix_Quit();
    TTF_Quit();
}

/* Source of the Files: 
Pieces: https://commons.wikimedia.org/wiki/Category:SVG_chess_pieces 
Sounds: https://www.chess.com/forum/view/general/chessboard-sound-files?page=2
Font: Fira Sans, from Google Fonts
*/
bool Graphics::loadMedia() {
    bool success = true;
    
    /* ##### White Pieces ##### */
    /* loadTexture returns true if it is successful, false otherwise. So if it is not true, it means it failed */

    if(!whitePieces[static_cast<int>(PieceType::Pawn)].loadTexture("../assets/pieces/default/white/WhitePawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Knight)].loadTexture("../assets/pieces/default/white/WhiteKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Bishop)].loadTexture("../assets/pieces/default/white/WhiteBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Rook)].loadTexture("../assets/pieces/default/white/WhiteRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::Queen)].loadTexture("../assets/pieces/default/white/WhiteQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!whitePieces[static_cast<int>(PieceType::King)].loadTexture("../assets/pieces/default/white/WhiteKing.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    
    /* ##### Black Pieces ##### */
    if(!blackPieces[static_cast<int>(PieceType::Pawn)].loadTexture("../assets/pieces/default/black/BlackPawn.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Knight)].loadTexture("../assets/pieces/default/black/BlackKnight.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Bishop)].loadTexture("../assets/pieces/default/black/BlackBishop.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Rook)].loadTexture("../assets/pieces/default/black/BlackRook.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::Queen)].loadTexture("../assets/pieces/default/black/BlackQueen.png", renderer)) {
        std::cerr << "Failed to load texture! ";
        success = false;
    }
    if(!blackPieces[static_cast<int>(PieceType::King)].loadTexture("../assets/pieces/default/black/BlackKing.png", renderer)) {
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
    int scaledFontSize = boardMarkingsFontSize * scaleY;
    boardFont = TTF_OpenFont("../assets/fonts/Fira_Sans/FiraSans-Medium.ttf", scaledFontSize);
    if (boardFont == nullptr) {
        std::cerr << "Failure to load boardFont! SDL_ttf Error: " << TTF_GetError() << std::endl;
        success = false;
    }

    scaledFontSize = statusFontSize * scaleY;
    statusFont = TTF_OpenFont("../assets/fonts/Fira_Sans/FiraSans-Medium.ttf", scaledFontSize);
    if (statusFont == nullptr) {
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

/* Clears all elements of the Window */
void Graphics::clearWindow() {
    SDL_SetRenderDrawColor(renderer, BKGD_COLOR.r, BKGD_COLOR.g, BKGD_COLOR.b, BKGD_COLOR.a);
    SDL_RenderClear(renderer);
}

/* Updates the Window */
void Graphics::updateWindow() {
    SDL_RenderPresent(renderer);
}

/* Renders a board square with the correct color, according to the row and column */
void Graphics::renderBoardSquare(int col, int row) {
    if (col < 0 || row < 0 || col >= COL || row >= ROW) return;   

    int index = Board::squareToIndex(row, col);
    const SDL_Rect& fillRect = squares[index];

    if ((row + col) % 2 != 0) {
        SDL_SetRenderDrawColor(renderer, WHITE_SQUARE.r, WHITE_SQUARE.g, WHITE_SQUARE.b, WHITE_SQUARE.a);
    } else {
        SDL_SetRenderDrawColor(renderer, BLACK_SQUARE.r, BLACK_SQUARE.g, BLACK_SQUARE.b, BLACK_SQUARE.a);
    }

    SDL_RenderFillRect(renderer, &fillRect);
}

/* Renders the letters and numbers on the side of the board, which is used for notation */
void Graphics::renderMarkings() {

    if(!isBoardFlipped) {
        /* File Markings */
        for (int i = 0; i < COL; ++i) {
            boardLetters[i].renderText(renderer, BOTTOM_BORDER_SIZE - 1 + (SQUARE_SIZE * i) + (static_cast<int>(SQUARE_SIZE/2) - boardLetters[i].getWidth()/(2*scaleX)), WIN_HEIGHT - BOTTOM_BORDER_SIZE + boardLetters[5].getWidth()/(2*scaleX), scaleY);
        }
        /* Row Markings */
        for (int i = 0; i < ROW; ++i) {
            boardNumbers[ROW - 1 - i].renderText(renderer, static_cast<int>(BORDER_SIZE/2) - boardNumbers[i].getWidth()/(2*scaleX), BORDER_SIZE - 1 + (SQUARE_SIZE * i) + (static_cast<int>(SQUARE_SIZE/2) - boardNumbers[i].getHeight()/(2*scaleY)), scaleY);
        }
    } else { /* Flipped Markings*/

        for (int i = 0; i < COL; ++i) {
            boardLetters[COL - 1 - i].renderText(renderer, BOTTOM_BORDER_SIZE - 1 + (SQUARE_SIZE * i) + (static_cast<int>(SQUARE_SIZE/2) - boardLetters[i].getWidth()/(2*scaleX)), WIN_HEIGHT - BORDER_SIZE + boardLetters[5].getWidth()/(2*scaleX), scaleY);
        }
        /* Row Markings */
        for (int i = 0; i < ROW; ++i) {
            boardNumbers[i].renderText(renderer, static_cast<int>(BORDER_SIZE/2) - boardNumbers[i].getWidth()/(2*scaleX), BORDER_SIZE - 1 + (SQUARE_SIZE * i) + (static_cast<int>(SQUARE_SIZE/2) - boardNumbers[i].getHeight()/(2*scaleY)), scaleY);
        }


    }
    
}

/* Renders the full board together with the markings */
void Graphics::renderBoard() {
    SDL_SetRenderDrawColor(renderer, BKGD_COLOR.r, BKGD_COLOR.g, BKGD_COLOR.b, BKGD_COLOR.a);
    SDL_RenderClear(renderer);

    for (int col = 0; col < COL; ++col) {
		for (int row = 0; row < ROW; ++row) {
			renderBoardSquare(col, row);
		}
        
	}
    if (showMarkings) renderMarkings();
}

/* Renders a piece placed in the board, in the position provided by index */
void Graphics::renderPiece(const Board & board, int index) {
    
    if(index < 0 && index > (COL*ROW - 1))
        return;
    
    Color color;
	PieceType piece;

    if(board.board[index] != nullptr) {
        color = board.board[index]->getColor();
        piece = board.board[index]->getType();
        int pieceID = static_cast<int>(piece);

        const SDL_Rect& dstRect = squares[index];

        if(piece == PieceType::King) {
            King * king = static_cast<King *>(board.board[index]);
            if (king->isChecked()) {
                kingInCheck.renderTexture(renderer, dstRect.x, dstRect.y);
            }
        }
        
        if (color == Color::White)
            whitePieces[pieceID].renderTexture(renderer, dstRect.x, dstRect.y);
        
        if (color == Color::Black)
            blackPieces[pieceID].renderTexture(renderer, dstRect.x, dstRect.y);
    }
}

/* Renders the marking which represents the King in check */
void Graphics::renderKingInCheck(int index) {
    const SDL_Rect dstRect = squares[index];
    kingInCheck.renderTexture(renderer, dstRect.x, dstRect.y);
    
}

/* Render all the pieces of the board */
void Graphics::renderPieces(const Board & board) {  
    for (int i = 0; i < COL * ROW; i++) {
		renderPiece(board, i);
	}
}

/* Highlights a square according to the index */
void Graphics::highlightSquare(int index) {
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, HIGHLIGHT.r, HIGHLIGHT.g, HIGHLIGHT.b, HIGHLIGHT.a);
	SDL_RenderFillRect(renderer, &squares[index]);
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

/* Places a dot in the provided index */
void Graphics::highlightMove(int index) {
    const SDL_Rect dstRect = squares[index];
    moveDot.renderTexture(renderer, dstRect.x, dstRect.y);
}

/* Places a marking of capture on the specified index */
void Graphics::highlightCapture(int index) {
    const SDL_Rect dstRect = squares[index];
    capture.renderTexture(renderer, dstRect.x, dstRect.y);
}

/* Highlights the possible moves of a piece in the board, according to the given index. The possible moves are generated by the respective pieces. Check the class Board methods to find more information about the Move Generation mechanism */
void Graphics::highlightPossibleMoves(const Board & board, int index) {
    if (index > 63 || index < 0) return;
    if (board.board[index] == nullptr) return;
    Piece * piece = board.board[index];

    for (int possibleMove : piece->validMoves) {
        if (board.board[possibleMove] != nullptr || possibleMove == board.getEnPassantIndex()) {
             highlightCapture(possibleMove);
        } else {
            highlightMove(possibleMove);
        }
    }
    
}

/* Selects a piece in board according to the index, which means highlighting it and showing all possible moves. It calls the methods defined in Graphics, in a sequential form, to allow a layering. SDL does not provide a layering system, which requires to manually create layers by rendering in order */
void Graphics::selectPiece(const Board & board, int index) {
    //clearWindow();
	renderBoard();
	highlightSquare(index);
	renderPieces(board);
    highlightPossibleMoves(board, index);
	//updateWindow();
}

/* Renders the full board with all the pieces according to the placement in a Board class */
void Graphics::renderBoardWithPieces(const Board & board) {
    //clearWindow();
    renderBoard();
    highlightAttacks(board); // New
	renderPieces(board);
	//updateWindow();
}

/* Renders a white marking on the square which is the piece is currently hovering */
void Graphics::renderHoverSquare(int mouseX, int mouseY) {
    int hoverCol, hoverRow;
    
    if(mouseX < (WIN_WIDTH - RIGHT_BORDER_SIZE) && (mouseX > LEFT_BORDER_SIZE) && mouseY < (WIN_HEIGHT - BOTTOM_BORDER_SIZE) && (mouseY > TOP_BORDER_SIZE)) {
        hoverCol = (mouseX - BORDER_SIZE) / SQUARE_SIZE;
        hoverRow = (mouseY - BORDER_SIZE) / SQUARE_SIZE;
    } else {
        hoverCol = -1;
        hoverRow = -1;
    }

    if (hoverCol > -1 && hoverRow > -1)
        hoverSquare.renderTexture(renderer, LEFT_BORDER_SIZE - 1 + SQUARE_SIZE * hoverCol, TOP_BORDER_SIZE - 1 + SQUARE_SIZE * hoverRow);
}



/* Renders the piece of the provided index, in the position of the mouse. It is only used when the Piece is being dragged, i.e., left mouse button pressed + movement of the mouse/keypad. It performs the layering and then renders the Piece on the given mouse position

Source: https://gigi.nullneuron.net/gigilabs/sdl2-drag-and-drop/?fbclid=IwY2xjawJcp9dleHRuA2FlbQIxMAABHVwbudUFVEK3WEu3RsJArnH2_GUbucPv5NFXbvub048pgzXka-PFcwqrIg_aem_hUfSUIs5p6Sm0uZ4gBEfHg */
void Graphics::renderDraggedPiece(const Board & board, int index, int mouseX, int mouseY) {

    Color color;
	PieceType piece;
    color = board.board[index]->getColor();
	piece = board.board[index]->getType();
    int pieceID = static_cast<int>(piece);

    //clearWindow();
	renderBoard();
    renderHoverSquare(mouseX, mouseY);
	renderPieces(board);
    highlightSquare(index);
    highlightPossibleMoves(board, index);

    if (color == Color::White)
		whitePieces[pieceID].renderTexture(renderer, mouseX - SQUARE_SIZE/2, mouseY - SQUARE_SIZE/2);
			
	if (color == Color::Black)
		blackPieces[pieceID].renderTexture(renderer, mouseX - SQUARE_SIZE/2, mouseY - SQUARE_SIZE/2);

    //updateWindow();
}

/* Renders a piece moving from the origin index to the destination index. It is called only if the user executed a move by pressing in a valid move square. I used the help of DeepSeek, which gave me the idea of using linear interpolation. It renders according to the given frames per seconds. As the screen of my computer is 120Hz, I set it up to render at 120fps */
void Graphics::animatePieceMoving(const Board & board, int fromIndex, int toIndex) {
    int fromCol = Board::indexToColumn(fromIndex);
    int fromRow = Board::indexToRow(fromIndex);

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
        //clearWindow();
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

        /* Render GUI while animating */
        gui->render();
        updateWindow();
        SDL_Delay(frameDelay);
    }
    
}

/* Prints a given text on the center of the screen. The Board parameter is necessary for the renderPieces method */
void Graphics::printText(const Board & board, std::string & text) {
    /* First, render the board normally */
    //clearWindow();
    renderBoard();
	renderPieces(board);

    /* Then, create the texture and render the text*/
    Texture renderText;
    renderText.loadFromRenderedText(renderer, boardFont, text, STATUS_TEXT);

    /* Center text*/
    int x = (WIN_WIDTH - renderText.getWidth())/2;
    int y = (WIN_HEIGHT - renderText.getHeight())/2;

    renderText.renderText(renderer, x, y);
    updateWindow();
}

/* Flips the board */
void Graphics::flipBoard() {

    isBoardFlipped = !isBoardFlipped;
    for (int row = 0; row < ROW; ++row) {
        for (int col = 0; col < COL; ++col) {
            int index = row * COL + col;
                
            if (isBoardFlipped) {
                int flippedRow = row;
                int flippedCol = (COL - 1) - col;

                squares[index] = {LEFT_BORDER_SIZE - 1 + flippedCol * SQUARE_SIZE, TOP_BORDER_SIZE - 1 + flippedRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            } else {
                int sdlRow = (ROW - 1) - row;
    
                squares[index] = {LEFT_BORDER_SIZE - 1 + col * SQUARE_SIZE, TOP_BORDER_SIZE - 1 + sdlRow * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE};
            }
        }
    }
    
}

/* Highlights attacks */
void Graphics::highlightAttacks(const Board & board) {
    
    for (int i = 0; i < 64; ++i) {
        if (whiteAttack) {
            if (board.whiteAttackBoard[i]) {
                highlightSquare(i);
            }
        }   
        if (blackAttack) {
            if (board.blackAttackBoard[i]) {
                highlightSquare(i);
            }
        }  
    }
}