#ifndef GRAPHICS_H
#define GRAPHICS_H

/* ##### Standard Libraries ##### */
#include <iostream>

/* ##### SDL Include ##### */
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

/* ##### Class Forward Declaration ##### */
class Board;
class ChessGUI;

/* ##### Global Variables for Board Properties ##### */
extern const int ROW;
extern const int COL;
extern int SQUARE_SIZE;
extern int BORDER_SIZE;
extern int WIN_WIDTH;
extern int WIN_HEIGHT;
extern SDL_Color WHITE_SQUARE;
extern SDL_Color BLACK_SQUARE;
extern SDL_Color BKGD_COLOR;
extern const SDL_Color DEFAULT_WHITE_SQUARE;
extern const SDL_Color DEFAULT_BLACK_SQUARE;
extern const SDL_Color DEFAULT_BKGD_COLOR;
extern int durationMs;

/* ##### Global Sound Effects ##### */
extern Mix_Chunk * gameStartSound;
extern Mix_Chunk * gameEndSound;
extern Mix_Chunk * captureSound;
extern Mix_Chunk * castleSound;
extern Mix_Chunk * moveSound;
extern Mix_Chunk * moveCheckSound;
extern Mix_Chunk * promoteSound;
extern Mix_Chunk * illegalMoveSound;

/* ##### Class #####*/
class Graphics {
public:
    /* RAII Technique - Resource Acquisition Is Initialization */
    Graphics();
    ~Graphics();
    
    /* Getters for the window and renderer, to be used if necessary */
    SDL_Window * getWindow() const { return mWindow; }
    SDL_Renderer * getRenderer() const { return mRenderer; }

    /* Set GUI Pointer (only used for animatePieceMoving)*/
    void setGUIPointer(ChessGUI * inGUI) { mGuiPtr = inGUI; }

    /* Media Loader */
    bool loadMedia();

    /* Board Flipping */
    bool isBoardFlipped;
    bool showMarkings;
    bool whiteAttack;
    bool blackAttack;

    /* GUI Methods */
    void clearWindow();
    void updateWindow();
    void renderBoardSquare(int col, int row);
    void renderBoard();
    void renderMarkings();
    void renderPiece(const Board & board, int index);
    void renderKingInCheck(int index);
    void renderPieces(const Board & board);
    void renderBoardWithPieces(const Board & board);
    void highlightSquare(int index);
    void highlightMove(int index);
    void highlightCapture(int index);
    void highlightAttacks(const Board & board);
    void highlightPossibleMoves(const Board & board, int index);
    void selectPiece(const Board & board, int index);
    void renderHoverSquare(int mouseX, int mouseY);
    void renderDraggedPiece(const Board & board, int index, int mouseX, int mouseY);
    void animatePieceMoving(const Board & board, int fromIndex, int toIndex);
    void printText(const Board & board, std::string & text);
    void flipBoard();

private:
    SDL_Window * mWindow;
    SDL_Renderer * mRenderer;
    // static bool instantiated; /* https://gameprogrammingpatterns.com/singleton.html */
    std::array<SDL_Rect, 64> mSquares;
    ChessGUI * mGuiPtr;
};

#endif