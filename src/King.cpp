#include "King.hpp"

King::King(Color color, int position) : Piece(color, PieceType::King, position) {
    kingSideCastle = false;
    queenSideCastle = false;
    inCheck = false;
    hasMoved = false;
}

