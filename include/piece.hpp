#ifndef PIECE_H
#define PIECE_H

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
        // virtual void possibleMoves();
        // virtual bool isValidMove();

        /* Virtual Destructor */
        // virtual ~Piece();

        /* Getters */
        int getPosition() const {return position;}
        int getRow() const {return position / 8;}
        int getColumn() const {return position % 8;}
        Color getColor() const {return color;}
        PieceType getType() const {return type;}
};


#endif