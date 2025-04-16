#include "King.hpp"

King::King(Color color, int position) : Piece(color, PieceType::King, position) {
    kingSideCastle = true;
    queenSideCastle = true;
    inCheck = false;
    hasMoved = false;
}

void King::setKingSideCastleRight(bool state) {
    kingSideCastle = state;
}

void King::setQueenSideCastleRight(bool state) {
    kingSideCastle = state;
}