#ifndef TEXTURE_H
#define TEXTURE_H

/* ##### SDL Include ##### */
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>

/* ##### Libraries ##### */
#include <iostream>

class Texture {
    private:
        SDL_Texture * mTexture;
        int mWidth;
        int mHeight;
    public:
        Texture();
        ~Texture();
        void free();
        bool loadTexture(std::string path, SDL_Renderer * renderer);
        bool loadFromRenderedText(SDL_Renderer * renderer, TTF_Font * font, std::string textureText, SDL_Color textColor);
        void renderTexture(SDL_Renderer * renderer, int x, int y);
        void renderText(SDL_Renderer * renderer, int x, int y, float scale = 1.0);
        int getWidth() const;
        int getHeight() const;
};

#endif