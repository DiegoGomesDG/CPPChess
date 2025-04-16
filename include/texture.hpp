#ifndef TEXTURE_H
#define TEXTURE_H

/* ##### SDL Include ##### */
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

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
        bool loadTexture(std::string path, SDL_Renderer * renderer);
        void renderTexture(SDL_Renderer * renderer, int x, int y);
        int getWidth() const;
        int getHeight() const;
};

#endif