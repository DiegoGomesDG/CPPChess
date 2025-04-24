#ifndef GRAPHICS_H
#define GRAPHICS_H

/* ##### Standard Libraries ##### */
#include <iostream>

/* ##### SDL Include ##### */
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <SDL2_ttf/SDL_ttf.h>

/* ##### Class Forward Declaration ##### */
class Board;

/* ##### Project headers ##### */
#include "Graphics.hpp"

/* Global Variables for Board Properties */
extern const int ROW;
extern const int COL;
extern const int SQUARE_SIZE;
extern const int BORDER_SIZE;
extern const int WIN_WIDTH;
extern const int WIN_HEIGHT;

/* Global Sound Effects */
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
    
    private:
        SDL_Window * window;
        SDL_Renderer * renderer;
        static bool instantiated; /* https://gameprogrammingpatterns.com/singleton.html */
        std::array<SDL_Rect, 64> squares;

    public:
        /* RAII Technique - Resource Acquisition Is Initialization */
        Graphics();
        ~Graphics();
        bool loadMedia();
        SDL_Window * getWindow() const {return window;};
        SDL_Renderer * getRenderer() const {return renderer;};

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
        void highlightPossibleMoves(const Board & board, int index);
        void selectPiece(const Board & board, int index);
        void renderHoverSquare(int mouseX, int mouseY);
        void renderDraggedPiece(const Board & board, int index, int mouseX, int mouseY);
        void animatePieceMoving(const Board & board, int fromIndex, int toIndex);
        
};

#endif