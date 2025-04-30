#ifndef GAME_H
#define GAME_H

#include "Graphics.hpp"
#include "Board.hpp"

enum class GameState {
    Idle,
    PieceSelected,
    Dragging,
    Processing,
    GameOver
};

enum class GameResult {
    Ongoing,
    WhiteWins,
    BlackWins,
    Draw
};

class ChessGame {
    private:
        GameState state;
        Board board;
        Graphics graphics;
        int focusIndex;
        int targetIndex;
        Color turn;
        bool leftMouseButtonDown;
        bool wasClicked;
    public:
        ChessGame(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq");

        void setTurn(Color color) {turn = color;}
        Color getTurn() const {return turn;}

        void handleStateTransition();
        void handleEvent(SDL_Event & event);
        void processMove();

        bool isGameOver() const {return state == GameState::GameOver;}
        Graphics * getGraphic() {return &graphics;}
};

#endif