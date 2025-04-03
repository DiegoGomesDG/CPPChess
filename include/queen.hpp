#ifndef QUEEN_H
#define QUEEN_H

#include "piece.hpp"

class Queen : public Piece {
    private:
    public:
        Queen(Color color, int position) : Piece(color, PieceType::Queen, position) {}    
};

#endif