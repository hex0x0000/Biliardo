#include "biliardo.hpp"
#include <SDL_image.h>

using std::shared_ptr;
using std::make_shared;
using std::string;
using namespace std::literals::string_literals;

Rendered::Rendered(SDL_Renderer *rend, SDL_Rect rect, SDL_Surface *surface) {
	this->rect = rect;
	this->texture = SDL_CreateTextureFromSurface(rend, surface);
	if (!this->texture)
		throw RenderingException("Failed to create texture");
}

Rendered::Rendered(SDL_Renderer *rend, int x, int y, unsigned int radius, char *img_path) {
	SDL_Rect rect = {.x = x, .y = y, .h = radius, .w = radius};
	SDL_Surface *surface = IMG_Load(img_path);
	if (!surface)
		throw RenderingException("Impossibile caricare l'immagine: "s + string(img_path) + " non trovato"s);
	this->texture = SDL_CreateTextureFromSurface(rend, surface);
	if(!this->texture)
		throw RenderingException("Failed to create texture");
	SDL_FreeSurface(surface);
}

Rendered::Rendered(SDL_Renderer *rend, SDL_Rect rect, char *img_path) {
	this->rect = rect;	
	SDL_Surface *surface = IMG_Load(img_path);
	if (!surface)
		throw RenderingException("Impossibile caricare l'immagine: "s + string(img_path) + " non trovato"s);
	this->texture = SDL_CreateTextureFromSurface(rend, surface);
	if(!this->texture)
		throw RenderingException("Failed to create texture");
	SDL_FreeSurface(surface);
}

void Rendered::render(SDL_Renderer *rend) {
	SDL_RenderCopy(rend, this->texture, NULL, &this->rect);
}

Rendered::~Rendered() {
	SDL_DestroyTexture(this->texture);
}

Pool::Pool(SDL_Renderer *rend, unsigned int height, unsigned int width, unsigned int balls_num) {
	for (int i = 0; i < balls_num; i++) {
		shared_ptr<Rendered> ball = make_shared<Ball>();
	}
}

void Pool:render(SDL_REnderer *rend) {
	SDL_SetRenderDrawColor(SDL_Renderer *renderer, 0, 128, 0, 1);
}

