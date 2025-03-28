#include "texture.hpp"

Texture::Texture() {
    this->texture = NULL;
    this->width = 0;
    this->height = 0;
}

Texture::~Texture() {

}

bool Texture::loadTexture(std::string path, SDL_Renderer * renderer) {
	
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
        std::cerr << "Unable to load image! SDL_Image Error: " << IMG_GetError() << std::endl;
    } else {
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		SDL_SetTextureScaleMode(newTexture, SDL_ScaleModeLinear);
		if (newTexture == NULL) {
            std::cerr << "Unable to create texture from " << path.c_str() << "!" << "SDL Error" << SDL_GetError() << std::endl;
		} else {
			this->width = loadedSurface->w;
			this->height = loadedSurface->h;
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	this->texture = newTexture;
	
	return texture != NULL;
}

void Texture::renderTexture(SDL_Renderer * renderer, int x, int y) {
	SDL_Rect renderQuad = {x, y, 90, 90}; // Change 90 to a variable
	SDL_RenderCopy(renderer, this->texture, NULL, &renderQuad);
}
