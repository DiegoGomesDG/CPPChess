#ifndef GAME_H
#define GAME_H

#include "Graphics.hpp"
#include "Board.hpp"

/* ##### Class Forward Declaration ##### */
class ChessGUI;

/* ##### Game States ##### */
enum class GameState {
    Idle,
    PieceSelected,
    Dragging,
    Promoting,
    Processing,
    GameOver,
};

class ChessGame {    
public:
    /* Constructor with a the initial position */
    ChessGame(const std::string& fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    /* Public Graphics */
    Graphics graphics;

    /* Set the GUI Pointer */
    void setGUIPointer(ChessGUI * inGUI) {mGuiPtr = inGUI;}

    /* Return State */
    GameState getState() const {return mState;}

    /* Public Move List */
    std::vector<std::string> moveList;

    /* Reset the Game */
    void resetGame();

    /* Load Position from FEN */
    bool loadFEN(const std::string fen);

    /* Turn Methods */
    void setTurn(Color color) {mTurn = color;}
    Color getTurn() const {return mTurn;}

    /* Half Move Clock Methods */
    void setHalfMoveClock(int value) {mHalfMoveClock = value;}
    int getHalfMoveClock() const {return mHalfMoveClock;}

    /* Full Move Clock Methods*/
    void setFullMoveClock(int value) {mFullMoveClock = value;}
    int getFullMoveClock() const {return mFullMoveClock;}

    /* Get En Passant Index */
    int getEnPassantIndex() const {return mBoard.getEnPassantIndex();}

    /* Get move count */
    int getMoveCount() const {return mBoard.moveCount;}

    /* Pieces */
    int getFocusIndex() const {return mFocusIndex;}
    int getTargetIndex() const {return mTargetIndex;}

    /* State transition, event handler, render handler */
    void handleStatesProcessing();
    void handleRender();
    void handleEvent(SDL_Event & event);

    /* State Handling */
    void handleProcessingMove();
    void handleGameOver();

    /* Game Status */
    bool isGameOver() const { return mState == GameState::GameOver; }

    /* File Handling and Register Moves */
    bool generatePGN(const std::string & result = "");
    void registerMove();

private:
    GameState mState;
    Board mBoard;
    ChessGUI * mGuiPtr;
    
    int mFocusIndex;
    int mTargetIndex;
    SDL_Point mMousePos;
    bool mLeftMouseButtonDown;
    bool mWasClicked;

    Color mTurn;
    int mHalfMoveClock;
    int mFullMoveClock;
    /* add std::unordered_map for tracking repetitions */
    
    bool mProcessGameOver;
};

#endif