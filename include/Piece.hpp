#ifndef PIECE_H
#define PIECE_H

/* Standard Libraries */
#include <vector>

/* ##### Class Forward Declaration ##### */
class Board;

enum class Color {White, Black};
enum class PieceType {Empty, Pawn, Knight, Bishop, Rook, Queen, King};

class Piece {
public:
    std::vector<int> validMoves;
    
    virtual Piece * clone(Board* newBoard) const = 0;
    virtual void computeMoves() = 0;
    bool isValidMove(int toIndex);

    /* Virtual Destructor */
    virtual ~Piece() {}

    /* Getters */
    int getPosition() const { return mPosition; }
    int getRow() const { return mPosition / 8; }
    int getColumn() const { return mPosition % 8; }
    bool getHasMoved() const { return mHasMoved; }
    Color getColor() const { return mColor; }
    PieceType getType() const { return mType; }

    /* Setters */
    void setPosition(int pos) { mPosition = pos; }
    void setHasMoved(int state) { mHasMoved = state; }

protected:
    int mPosition;
    Color mColor;
    PieceType mType;
    Board * mBoard;
    bool mHasMoved;

    /* Protected Constructor*/
    Piece(Color color, PieceType type, int position, Board * board, bool hasMoved) : mPosition(position), mColor(color), mType(type), mBoard(board), mHasMoved(hasMoved) {}
};


#endif