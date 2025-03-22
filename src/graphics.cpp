#include "graphics.hpp"

const int SQUARE_SIZE = 60;
const int BORDER_SIZE = 10;
const int WIN_WIDTH = 8 * SQUARE_SIZE + 2 * BORDER_SIZE;
const int WIN_HEIGTH = 8 * SQUARE_SIZE + 2 * BORDER_SIZE;

/* ----- Board Properties ----- */
const SDL_Color WHITE_SQUARE = {0xFF, 0xFF, 0xFF, 0xFF};
const SDL_Color BLACK_SQUARE = {0x00, 0x00, 0x00, 0xFF};
const SDL_Color BKGD_COLOR = {0xA4, 0x74, 0x49, 0xFF};

