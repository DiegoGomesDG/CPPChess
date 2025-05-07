#include "ChessGUI.hpp"
#include "Board.hpp"
#include "Graphics.hpp"
#include "SDL_timer.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

/* Constructor of Class Members */
ChessGUI::ChessGUI(SDL_Window * window, SDL_Renderer * renderer, ChessGame * game) : mWindow(window), mRenderer(renderer), mGame(game) {
    showDemoWindow = false;
}

/* Destructor and Clean-Up */
ChessGUI::~ChessGUI() {
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

/* helpMarker, from https://github.com/ocornut/imgui/blob/master/imgui_demo.cpp */
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

/* Initialize imgui */
void ChessGUI::init() {

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    /* Initialize imgui with SDL_Renderer backend */
    ImGui_ImplSDL2_InitForSDLRenderer(mWindow, mRenderer);
    ImGui_ImplSDLRenderer2_Init(mRenderer);

    ImGui::StyleColorsDark();
}

/* Render */
void ChessGUI::render() {
    
    /* Start the Dear ImGui frame */
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    /* Default placement of the window */
    ImGui::SetNextWindowPos(ImVec2(BORDER_SIZE - 5, BORDER_SIZE - 5), ImGuiCond_FirstUseEver);
    ImGui::NewFrame();

    {
        /* Begin Window */
        ImGui::Begin("CPPChess GUI");

        /* Show demo window if triggered */
        if (showDemoWindow)
            ImGui::ShowDemoWindow(&showDemoWindow);

        /* Info Section */
        if (ImGui::CollapsingHeader("Info")) {
            ImGuiIO& io = ImGui::GetIO();
            
            /* Turn*/
            ImGui::Text("Turn: %s", mGame->getTurn() == Color::White ? "White" : "Black");

            /* Display informations about the Mouse */
            if (ImGui::TreeNode("Mouse")) {
                /* Get Mouse Position*/
                if (ImGui::IsMousePosValid())
                    ImGui::Text("Mouse pos: (%g, %g)", io.MousePos.x, io.MousePos.y);
                else
                    ImGui::Text("Mouse pos: <INVALID>");

                /* Mouse Dragging*/
                ImGui::Text("Mouse delta: (%g, %g)", io.MouseDelta.x, io.MouseDelta.y);
                ImGui::TreePop();
            }

            /* Display informations about which piece is focused, the target square and target square for the en-passant move */
            if (ImGui::TreeNode("Squares")) {
                /* Focused Piece */
                ImGui::Text("Focused Piece: %s", Board::indexToAlgebraic(mGame->getFocusIndex()).c_str());

                /* Target Square */
                ImGui::Text("Target Square: %s", Board::indexToAlgebraic(mGame->getTargetIndex()).c_str());

                /* En Passant Index */
                ImGui::Text("En Passant: %s", Board::indexToAlgebraic(mGame->getEnPassantIndex()).c_str());
                ImGui::TreePop();
            }

            /* Show the move clocks counter */
            if (ImGui::TreeNode("Move Clocks")) {
                /* Full Move Clock */
                ImGui::Text("Full Move Clock: %d", mGame->getFullMoveClock());

                /* Half Move Clock */
                ImGui::Text("Half Move Clock: %d", mGame->getHalfMoveClock());
                ImGui::TreePop();
            }
            
            /* Display which keys are being pressed */
            if (ImGui::TreeNode("Keys")) {
                /*Get Key Down*/
                struct funcs { static bool IsLegacyNativeDupe(ImGuiKey) { return false; } };
                ImGuiKey start_key = ImGuiKey_NamedKey_BEGIN;
                ImGui::Text("Keys down:");         for (ImGuiKey key = start_key; key < ImGuiKey_NamedKey_END; key = (ImGuiKey)(key + 1)) { if (funcs::IsLegacyNativeDupe(key) || !ImGui::IsKeyDown(key)) continue; ImGui::SameLine(); ImGui::Text((key < ImGuiKey_NamedKey_BEGIN) ? "\"%s\"" : "\"%s\" %d", ImGui::GetKeyName(key), key); }

                ImGui::TreePop();
            }            

        }

        /* Load a positon or reset the board*/
        if (ImGui::CollapsingHeader("Load Position")) {
            ImGui::SeparatorText("Load from FEN");
            static char fenBuffer[128] = "";
            ImGui::InputTextWithHint("FEN", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", fenBuffer, IM_ARRAYSIZE(fenBuffer));

            if (ImGui::Button("Load FEN")) {
                std::string fenStr(fenBuffer);
                if (mGame->loadFEN(fenStr)) {
                    ImGui::SameLine();
                    ImGui::Text("Position Initialized");
                } else {
                    ImGui::OpenPopup("Invalid FEN");
                }
            }
            
            // Error Popup
            if (ImGui::BeginPopupModal("Invalid FEN")) {
                ImGui::Text("Invalid FEN format!");
                if (ImGui::Button("OK")) ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::SeparatorText("Reset Board");
            if (ImGui::Button("Reset Board")) {
                mGame->resetGame();
                ImGui::SameLine();
                ImGui::Text("Reset Done");
            }
        }   

        /* Settings of the Board */
        if (ImGui::CollapsingHeader("Board Settings")) {
            if (ImGui::Button("Flip Board")) /* Flip Board*/
                mGame->graphics.flipBoard();

            ImGui::Checkbox("Hide Markings", &mGame->graphics.showMarkings); /* Hide the ranks and files markings (a...h) and (1...8) */

            ImGui::DragInt("animation ms", &durationMs, 1); /* Change the speed of the move animations */
            ImGui::SameLine(); HelpMarker(
                "Click and drag to edit value.\n"
                "Hold SHIFT/ALT for faster/slower edit.\n"
                "Double-click or CTRL+click to input value.");
        }
        
        /* Change Board Colors*/
        if (ImGui::CollapsingHeader("Board Colors")) {

            ImVec4 white = ImVec4(WHITE_SQUARE.r/255.0, WHITE_SQUARE.g/255.0, WHITE_SQUARE.b/255.0, WHITE_SQUARE.a/255.0);
            ImVec4 black = ImVec4(BLACK_SQUARE.r/255.0, BLACK_SQUARE.g/255.0, BLACK_SQUARE.b/255.0, BLACK_SQUARE.a/255.0);
            ImVec4 bkgd = ImVec4(BKGD_COLOR.r/255.0, BKGD_COLOR.g/255.0, BKGD_COLOR.b/255.0, BKGD_COLOR.a/255.0);

            ImGui::ColorEdit3("White Square", (float*)&white);
            ImGui::ColorEdit3("Black Square", (float*)&black);
            ImGui::ColorEdit3("Background", (float*)&bkgd);

            /* Set Colors Accordingly */
            WHITE_SQUARE = {(Uint8)(white.x * 255), (Uint8)(white.y * 255), (Uint8)(white.z * 255), (Uint8)(white.w * 255)};
            BLACK_SQUARE = {(Uint8)(black.x * 255), (Uint8)(black.y * 255), (Uint8)(black.z * 255), (Uint8)(black.w * 255)};
            BKGD_COLOR = {(Uint8)(bkgd.x * 255), (Uint8)(bkgd.y * 255), (Uint8)(bkgd.z * 255), (Uint8)(bkgd.w * 255)};
            
            if (ImGui::Button("Reset to Default")) {
                WHITE_SQUARE = DEFAULT_WHITE_SQUARE;
                BLACK_SQUARE = DEFAULT_BLACK_SQUARE;
                BKGD_COLOR = DEFAULT_BKGD_COLOR;
            }

        }

        /* Print the move history and save PGN if requested by the user*/
        if (ImGui::CollapsingHeader("Move History")) {
            /* Generate PGN */
            if (ImGui::Button("Generate PGN")) {
                mGame->generatePGN("");
                ImGui::SameLine();
                ImGui::Text("PGN Generated");
            }

            /* Write the Moves into the imgui window */
            {
                static int max_height_in_lines = 10;
                ImGui::SetNextItemWidth(ImGui::GetFontSize() * 8);
                ImGui::DragInt("Max Height (in Lines)", &max_height_in_lines, 0.2f);
    
                ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 1), ImVec2(FLT_MAX, ImGui::GetTextLineHeightWithSpacing() * max_height_in_lines));
                
                if (ImGui::BeginChild("ConstrainedChild", ImVec2(-FLT_MIN, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY)) {
                    std::string moveList;
                    for (const std::string & move : mGame->moveList) {
                        moveList += move + " ";
                    }
                    ImGui::TextWrapped("%s", moveList.c_str());
                }
                ImGui::EndChild();
            }    
        }

        /* Other stuff such as demo window */
        if (ImGui::CollapsingHeader("Others")) {
            ImGui::Checkbox("Demo Window", &showDemoWindow);
        }

        ImGui::End();
    }

    /* Submit for Rendering, which will be updated in the main */
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), mRenderer);
}