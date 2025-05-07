#ifndef GUI_H
#define GUI_H

/* Imports */
#include "Graphics.hpp"
#include "Game.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"



class ChessGUI {

    private:
        bool showDemoWindow;
        SDL_Window * mWindow;
        SDL_Renderer * mRenderer;
        ChessGame * mGame;
    public:
        ChessGUI(SDL_Window * window, SDL_Renderer * renderer, ChessGame * game);
        ~ChessGUI();

        /* Initialize imgui */
        void init();

        /* Render */
        void render();


    

};

#endif