/* Includes */
#include "../include/Game.hpp"
#include "../include/Graphics.hpp"
#include "../include/ChessGUI.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"

/* find src include -name "*.cpp" -o -name "*.hpp" | xargs wc -l */

/* Check Version Requirements */
#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

/* Main Code */
int main(int argc, char * argv[]) {

    try {
        /* Create a Game */
        ChessGame game;

        /* Get the respective renderer and window to initialize the imgui */
        SDL_Renderer * renderer = game.graphics.getRenderer();
        SDL_Window * window = game.graphics.getWindow();

        /* Create the GUI and initiate it */
        ChessGUI gui(window, renderer, &game);
        gui.init();
        game.setGUIPointer(&gui);
        game.graphics.setGUIPointer(&gui);
        
        /* Main Game Loop */
        ImGuiIO& io = ImGui::GetIO(); (void)io; /* Get imgui i/o */
        bool quit = false; /* Flag */
        while (!quit) { /* Run loop till the program is terminated by the user*/

            game.graphics.clearWindow(); /* Clear the window */
            game.handleRender(); /* Render the board */
            
            /* Handle Events */
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                ImGui_ImplSDL2_ProcessEvent(&event);
                if (event.type == SDL_QUIT)
                    quit = true;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                    quit = true;
                if (io.WantCaptureKeyboard) continue; /* Ignore events which are directed to the GUI*/
                if (io.WantCaptureMouse) continue;
                
                game.handleEvent(event); /* Handle the events directed to the board */
            }

            gui.render(); /* Render the GUI on top of the Board */
            SDL_RenderPresent(renderer); /* Update the Window */
        }
    }
    catch(const std::runtime_error& e) {
        std::cerr << "[ERROR] Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch(...) {
        std::cerr << "[ERROR] Unknown error caught!" << std::endl;
    }
    

    return 0;
}