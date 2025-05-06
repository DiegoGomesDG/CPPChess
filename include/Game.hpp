#ifndef GAME_H
#define GAME_H

#include "Graphics.hpp"
#include "Board.hpp"
#include <fstream>

enum class GameState {
    Idle,
    PieceSelected,
    Dragging,
    Processing,
    GameOver,x3
};

class ChessGame {
    
    private:
        GameState state;
        Board board;
        int focusIndex;
        int targetIndex;
        SDL_Point mousePos;
        Color turn;
        int halfMoveClock;
        int fullMoveClock;
        bool leftMouseButtonDown;
        bool wasClicked;
        std::vector<std::string> moveList;
        
    public:

        /* Constructor with a the initial position */
        ChessGame(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        /* Public Graphics */
        Graphics graphics;

        /* Reset the Game */
        void resetGame();

        /* Turn Methods */
        void setTurn(Color color) {turn = color;}
        Color getTurn() const {return turn;}

        /* Half Move Clock Methods */
        void setHalfMoveClock(int value) {halfMoveClock = value;}
        int getHalfMoveClock() const {return halfMoveClock;}

        /* Full Move Clock Methods*/
        void setFullMoveClock(int value) {fullMoveClock = value;}
        int getFullMoveClock() const {return fullMoveClock;}

        /* State transition, event handler, render handler */
        void handleEvent(SDL_Event & event);
        void handleRender();
        void processMove();

        /* Game Status */
        bool isGameOver() const {return state == GameState::GameOver;}

        /* File Handling and Register Moves */
        bool generatePGN(const std::string & result = "");
        void registerMove();
};

#endif