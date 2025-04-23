#ifndef GRAPHICS_H
#define GRAPHICS_H

/* ##### SDL Include ##### */
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

/* ##### Class Forward Declaration ##### */
class Board;

/* ##### Project headers ##### */
#include "Graphics.hpp"

/* Global Variables */
extern const int ROW;
extern const int COL;
extern const int SQUARE_SIZE;
extern const int BORDER_SIZE;
extern const int WIN_WIDTH;
extern const int WIN_HEIGHT;

/* ##### Class #####*/
class Graphics {
    
    private:
        SDL_Window * window;
        SDL_Renderer * renderer;
        static bool instantiated; /* https://gameprogrammingpatterns.com/singleton.html */

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
        void renderPiece(const Board & board, int index);
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