#include "Texture.hpp"
#include "Graphics.hpp"


Texture::Texture() {
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

Texture::~Texture() {
	free();
}

void Texture::free() {
	//Free texture if it exists
	if(mTexture != nullptr) {
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}
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

bool Texture::loadFromRenderedText(SDL_Renderer * renderer, TTF_Font * font, std::string textureText, SDL_Color textColor) {
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, textureText.c_str(), textColor);
	if(textSurface == nullptr) {
		std::cout << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
	} else {
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		if(mTexture == nullptr) {
			std::cout << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
		} else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface(textSurface);
	}
	
	//Return success
	return mTexture != nullptr;
}

void Texture::renderTexture(SDL_Renderer * renderer, int x, int y) {
	SDL_Rect renderQuad = {x, y, SQUARE_SIZE, SQUARE_SIZE}; // Change 90 to a variable
	SDL_RenderCopy(renderer, this->mTexture, nullptr, &renderQuad);
}

void Texture::renderText(SDL_Renderer * renderer, int x, int y, float scale) {
	SDL_Rect renderQuad = {x, y, static_cast<int>(mWidth/scale), static_cast<int>(mHeight/scale)}; // 2 for macOS displays
	SDL_RenderCopy(renderer, this->mTexture, nullptr, &renderQuad);
}

int Texture::getWidth() const {
	return mWidth;
}

int Texture::getHeight() const {
	return mHeight;
}