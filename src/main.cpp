#include "../include/Game.hpp"

//Using SDL here for the SDL_Event
#include <SDL2/SDL.h>
#include <cmath>
/* find src include -name "*.cpp" -o -name "*.hpp" | xargs wc -l */

/* imgui includes */
#include "Graphics.hpp"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

// Main code
int main(int argc, char * argv[]) {

    ChessGame game;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::GetCurrentContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls  
    

    // Setup Platform/Renderer backends
    SDL_Renderer * renderer = game.graphics.getRenderer();
    SDL_Window * window = game.graphics.getWindow();

    // Our state
    bool show_demo_window = false;
    
    ImVec4 change_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool quit = false;
    while (!quit)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
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

        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }


        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("CPPChess GUI");                          

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            if (ImGui::CollapsingHeader("Configuration")) {
                ImGui::ColorEdit3("White Square", (float*)&change_color); // Edit 3 floats representing a color

                WHITE_SQUARE = {(Uint8)(change_color.x * 255), (Uint8)(change_color.y * 255), (Uint8)(change_color.z * 255), (Uint8)(change_color.w * 255)};
            }
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Show Markings", &game.graphics.showMarkings); 

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            

            if (ImGui::Button("Reset Game"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                game.resetGame();
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}