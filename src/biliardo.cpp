#include "biliardo.hpp"
#include <SDL_image.h>

using std::shared_ptr;
using std::make_shared;
using std::string;
using namespace std::literals::string_literals;

RenderingException::RenderingException(string msg) {
	this->msg = msg;
}

const char *RenderingException::what() const throw() {
	return msg.c_str();
}

Rendered::Rendered(SDL_Renderer *rend, SDL_Rect rect, SDL_Surface *surface) {
	this->rect = rect;
	this->texture = SDL_CreateTextureFromSurface(rend, surface);
	if (!this->texture)
		throw RenderingException("Impossibile creare texture");
}

Rendered::Rendered(SDL_Renderer *rend, int x, int y, int radius, string img_path) {
	this->rect = {.x = x, .y = y, .w = radius, .h = radius};
	SDL_Surface *surface = IMG_Load(img_path.c_str());
	if (!surface)
		throw RenderingException("Impossibile caricare l'immagine: "s + img_path + " non trovato"s);
	this->texture = SDL_CreateTextureFromSurface(rend, surface);
	if(!this->texture)
		throw RenderingException("Impossibile creare texture");
	SDL_FreeSurface(surface);
}

Rendered::Rendered(SDL_Renderer *rend, SDL_Rect rect, string img_path) {
	this->rect = rect;	
	SDL_Surface *surface = IMG_Load(img_path.c_str());
	if (!surface)
		throw RenderingException("Impossibile caricare l'immagine: "s + img_path + " non trovato"s);
	this->texture = SDL_CreateTextureFromSurface(rend, surface);
	if(!this->texture)
		throw RenderingException("Impossibile creare texture");
	SDL_FreeSurface(surface);
}

void Rendered::render(SDL_Renderer *rend) {
	SDL_RenderCopy(rend, this->texture, NULL, &this->rect);
}

Rendered::~Rendered() {
	SDL_DestroyTexture(this->texture);
}

Ball::Ball(SDL_Renderer *rend, int x, int y, int radius, std::string img_path, float friction, float mass)
	: Rendered(rend, x, y, radius, img_path) {
	this->vel_x = 0;
	this->vel_y = 0;
	this->angle = 0;
	this->friction = friction;
	this->mass = mass;
	this->radius = radius;
}

Pool::Pool(SDL_Renderer *rend, int height, int width, int balls_num) {
	this->pool = {.x = 0, .y = 200, .w = width, .h = height};
	for (int i = 0; i < balls_num; i++) {
		shared_ptr<Ball> ball = make_shared<Ball>(rend, 500, 500, 75, "test.jpg", 0.3, 1);
		this->balls.push_back(ball);
		this->elements.push_back(ball);
	}
}

void Pool::render(SDL_Renderer *rend) {
	SDL_SetRenderDrawColor(rend, 0, 128, 0, 1);
	SDL_RenderFillRect(rend, &this->pool);
	for (shared_ptr<Rendered> &element : this->elements) {
		element->render(rend);
	}
}

