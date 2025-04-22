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

        /* Protected Constructor*/
        Piece(Color color, PieceType type, int position) : position(position), color(color), type(type) {}

    public:
        std::vector<int> validMoves;

        virtual void possibleMoves(const Board & board) = 0;
        //virtual bool isValidMove();

        /* Virtual Destructor */
        virtual ~Piece() {}

        /* Getters */
        int getPosition() const {return position;}
        int getRow() const {return position / 8;}
        int getColumn() const {return position % 8;}
        Color getColor() const {return color;}
        PieceType getType() const {return type;}
};


#endif