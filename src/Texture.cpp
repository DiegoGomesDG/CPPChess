#include "Texture.hpp"
#include "Graphics.hpp"


Texture::Texture() {
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

Texture::~Texture() {
	mTexture = nullptr;
}

bool Texture::loadTexture(std::string path, SDL_Renderer * renderer) {
	
	//The final texture
	SDL_Texture* newTexture = nullptr;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image! SDL_Image Error: " << IMG_GetError() << std::endl;
    } else {
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		SDL_SetTextureScaleMode(newTexture, SDL_ScaleModeLinear);
		if (newTexture == nullptr) {
            std::cerr << "Unable to create texture from " << path.c_str() << "!" << "SDL Error" << SDL_GetError() << std::endl;
		} else {
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	this->mTexture = newTexture;
	
	return mTexture != nullptr;
}

void Texture::renderTexture(SDL_Renderer * renderer, int x, int y) {
	SDL_Rect renderQuad = {x, y, SQUARE_SIZE, SQUARE_SIZE}; // Change 90 to a variable
	SDL_RenderCopy(renderer, this->mTexture, nullptr, &renderQuad);
}


int Texture::getWidth() const {
	return mWidth;
}

int Texture::getHeight() const {
	return mHeight;
}