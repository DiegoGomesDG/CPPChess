/* find src include -name "*.cpp" -o -name "*.hpp" | xargs wc -l */
/* Includes */
#include "../include/Game.hpp"
#include "../include/Graphics.hpp"
#include "../include/ChessGUI.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

// Main code
int main(int argc, char * argv[]) {

    ChessGame game;

    // Setup Platform/Renderer backends
    SDL_Renderer * renderer = game.graphics.getRenderer();
    SDL_Window * window = game.graphics.getWindow();

    ChessGUI gui(window, renderer, &game);
    gui.init();
    
    // Main loop
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    bool quit = false;
    while (!quit) {

        game.graphics.clearWindow();
        game.handleRender();
        
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                quit = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                quit = true;
            if (io.WantCaptureKeyboard) continue;
            if (io.WantCaptureMouse) continue;
            
            game.handleEvent(event);
        }

        gui.render();
        SDL_RenderPresent(renderer);
    }

    return 0;
}