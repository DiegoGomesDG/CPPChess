#ifndef GRAPHICS_H
#define GRAPHICS_H

/* ##### SDL Include ##### */
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

/* ##### Project Files ##### */
#include "texture.hpp"
#include "piece.hpp"

/* ##### Libraries ##### */


extern Texture whitePieces[7];
extern Texture blackPieces[7];

/* ##### Class #####*/
class Graphics {
    private:
        SDL_Window * window;
        SDL_Renderer * renderer;
    public:
        /* RAII Technique - Resource Acquisition Is Initialization */
        Graphics();
        ~Graphics();
        bool loadMedia();
        SDL_Window * getWindow();
        SDL_Renderer * getRenderer();
};

#endif