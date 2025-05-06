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
    GameOver,
    Quit
};

class ChessGame {
    
    private:
        GameState state;
        Board board;
        Graphics graphics;
        int focusIndex;
        int targetIndex;
        Color turn;
        int halfMoveClock;
        int fullMoveClock;
        bool leftMouseButtonDown;
        bool wasClicked;
        std::vector<std::string> moveList;
        
    public:

        /* Constructor with a the initial position */
        ChessGame(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

        /* Get Renderer */
        

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

        /* State transition, event handler */
        void handleEvent(SDL_Event & event);
        void processMove();

        /* Game Status */
        bool isGameOver() const {return state == GameState::GameOver;}
        bool isQuit() const {return state == GameState::Quit;}

        /* File Handling and Register Moves */
        bool generatePGN(const std::string & result = "");
        void registerMove();
};

#endif