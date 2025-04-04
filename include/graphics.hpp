#ifndef GRAPHICS_H
#define GRAPHICS_H

/* ##### SDL Include ##### */
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

/* ##### Class Forward Declaration ##### */
class Board;

/* ##### Local headers ##### */
#include "texture.hpp"
#include "graphics.hpp"


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
        void renderBoard();
        void renderPieces(const Board & board);
        void dragPiece(const Board & board, int index, int mouseX, int mouseY);
};


#endif