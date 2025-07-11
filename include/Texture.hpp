#ifndef TEXTURE_H
#define TEXTURE_H

/* ##### SDL Include ##### */
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

/* ##### Libraries ##### */
#include <iostream>

class Texture {
public:
    Texture();
    ~Texture();
    void free();
    bool loadTexture(std::string path, SDL_Renderer * renderer);
    bool loadFromRenderedText(SDL_Renderer * renderer, TTF_Font * font, const std::string & textureText, SDL_Color textColor);
    void renderTexture(SDL_Renderer * renderer, int x, int y);
    void renderText(SDL_Renderer * renderer, int x, int y, float scale = 1.0);
    int getWidth() const;
    int getHeight() const;

private:
    SDL_Texture * mTexture;
    int mWidth;
    int mHeight;
};

#endif