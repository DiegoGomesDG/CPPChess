#ifndef PIECE_H
#define PIECE_H

/* Standard Libraries */
#include <vector>

/* ##### Class Forward Declaration ##### */
class Board;

enum class Color {White, Black};
enum class PieceType {Empty, Pawn, Knight, Bishop, Rook, Queen, King};

class Piece {

    protected:
        int position;
        Color color;
        PieceType type;
        Board * board;
        bool hasMoved;

        /* Protected Constructor*/
        Piece(Color color, PieceType type, int position, Board * board, bool hasMoved) : position(position), color(color), type(type), board(board), hasMoved(hasMoved) {}

    public:
        std::vector<int> validMoves;
        
        virtual Piece * clone(Board* newBoard) const = 0;
        virtual void computeMoves() = 0;
        bool isValidMove(int toIndex);

        /* Virtual Destructor */
        virtual ~Piece() {}

        /* Getters */
        int getPosition() const {return position;}
        int getRow() const {return position / 8;}
        int getColumn() const {return position % 8;}
        bool getHasMoved() const {return hasMoved;}
        Color getColor() const {return color;}
        PieceType getType() const {return type;}

        /* Setters */
        void setPosition(int pos) {position = pos;}
        void setHasMoved(int state) {hasMoved = state;}

};


#endif