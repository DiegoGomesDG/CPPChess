#ifndef GUI_H
#define GUI_H

/* Imports */
#include "Game.hpp"

class ChessGUI {

    private:
        bool showDemoWindow;
        bool showGameOver;
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

        /* Game Over Menu */
        void gameOverMenu();

};

#endif