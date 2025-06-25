#ifndef GAME_H
#define GAME_H

#include "Graphics.hpp"
#include "Board.hpp"

/* ##### Class Forward Declaration ##### */
class ChessGUI;

enum class GameState {
    Idle,
    PieceSelected,
    Dragging,
    Processing,
    GameOver,
};

class ChessGame {
    
    private:
        GameState state;
        Board board;
        ChessGUI * gui;
        int focusIndex;
        int targetIndex;
        SDL_Point mousePos;
        Color turn;
        int halfMoveClock;
        int fullMoveClock;
        bool leftMouseButtonDown;
        bool wasClicked;
        bool processGameOver;
    public:

        /* Constructor with a the initial position */
        ChessGame(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        /* Public Graphics */
        Graphics graphics;

        /* Set the GUI Pointer */
        void setGUIPointer(ChessGUI * inGUI) {gui = inGUI;}

        /* Return State */
        GameState getState() const {return state;}

        /* Public Move List */
        std::vector<std::string> moveList;

        /* Reset the Game */
        void resetGame();

        /* Load Position from FEN */
        bool loadFEN(const std::string fen);

        /* Turn Methods */
        void setTurn(Color color) {turn = color;}
        Color getTurn() const {return turn;}

        /* Half Move Clock Methods */
        void setHalfMoveClock(int value) {halfMoveClock = value;}
        int getHalfMoveClock() const {return halfMoveClock;}

        /* Full Move Clock Methods*/
        void setFullMoveClock(int value) {fullMoveClock = value;}
        int getFullMoveClock() const {return fullMoveClock;}

        /* Get En Passant Index */
        int getEnPassantIndex() const {return board.getEnPassantIndex();}

        /* Get move count */
        int getMoveCount() const {return board.moveCount;}

        /* Pieces */
        int getFocusIndex() const {return focusIndex;}
        int getTargetIndex() const {return targetIndex;}

        /* State transition, event handler, render handler */
        void handleStatesProcessing();
        void handleRender();
        void handleEvent(SDL_Event & event);

        /* State Handling */
        void handleProcessingMove();
        void handleGameOver();

        /* Game Status */
        bool isGameOver() const {return state == GameState::GameOver;}

        /* File Handling and Register Moves */
        bool generatePGN(const std::string & result = "");
        void registerMove();
};

#endif