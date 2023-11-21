#include "biliardo.hpp"
#include <SDL_image.h>
#include <cmath>
#include <iostream>

using std::cout;
using std::endl;

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::set;
using std::make_pair;
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
	this->rect = {.x = x, .y = y, .w = radius*2, .h = radius*2};
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

Ball::Ball(SDL_Renderer *rend, int x, int y, int radius, std::string img_path, float friction, float mass, double vel_x, double vel_y)
	: Rendered(rend, x, y, radius, img_path) {
	this->vel_x = vel_x;
	this->vel_y = vel_y;
	this->friction = friction;
	this->mass = mass;
	this->radius = radius;
	this->x = x;
	this->y = y;
	this->rect.x -= radius;
	this->rect.y -= radius;
}

void Ball::movement(float time) {
	before_x = x;
	before_y = y;
	x += vel_x * time;
	y += vel_y * time;
	rect.x = x - radius;
	rect.y = y - radius;
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;
	cout << "vel x: " << this->vel_x << endl;
	cout << "vel y: " << this->vel_y << endl;
	cout << "delta x: " << this->vel_x*time << endl;
	cout << "delta y: " << this->vel_y*time << endl << endl;
}

bool Ball::is_colliding(const shared_ptr<Ball> &ball) {
	double distance = sqrt(pow(ball->x - this->x, 2) + pow(ball->y - this->y, 2));
	cout << "distance: " << distance << endl;
	cout << "radius sum: " << ball->radius + this->radius << endl;
	if ((int)(this->radius + ball->radius) > distance) {
		this->x = this->before_x;
		this->y = this->before_y;
		ball->x = ball->before_x;
		ball->y = ball->before_y;
		return true;
	}
	return false;
}

void Ball::collide(std::shared_ptr<Ball> &ball) {
	double modulo = sqrt(pow(ball->x + this->x, 2) + pow(ball->y + this->y, 2));
	double unx = (this->x - ball->x)/modulo;
	double uny = (this->y - ball->y)/modulo;
	double utx = -uny;
	double uty = unx;
	double v1n = unx*this->vel_x+uny*this->vel_y;
	double v1t = utx*this->vel_x+uty*this->vel_y;
	double v2n = unx*ball->vel_x+uny*ball->vel_y;
	double v2t = utx*ball->vel_x+uty*ball->vel_y;
	double masses = this->mass + ball->mass;	
	double delta_mass = this->mass - ball->mass;
	double v1nf = ((v1n*delta_mass)+2*ball->mass*v2n)/masses;
	double v2nf = ((v2n*(-delta_mass))+2*this->mass*v1n)/masses;
	this->vel_x = v1nf*unx+v1t*utx;
	this->vel_y = v1nf*uny+v1t*uty;
	ball->vel_x = v2nf*unx+v2t*utx;
	ball->vel_y = v2nf*uny+v2t*uty;
}

void gen_combinations(set<pair<int, int>> &combinations, int size) {
	combinations.clear();
	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (j == i)
				continue;
			if (!combinations.contains(make_pair(j, i))) {
				combinations.insert(make_pair(i, j));
			}
		}
	}
}

Pool::Pool(SDL_Renderer *rend, int height, int width, int balls_num) {
	this->pool = {.x = 0, .y = 200, .w = width, .h = height};
	/*for (int i = 0; i < balls_num; i++) {
		shared_ptr<Ball> ball = make_shared<Ball>(rend, 500, 500, 75, "test.jpg", 0.3, 1);
		this->balls.push_back(ball);
		this->elements.push_back(ball);
	}*/
	
	shared_ptr<Ball> ball = make_shared<Ball>(rend, 400, 400, 50, "test.png", 5, 1, 0.08, 0.13);
	this->balls.push_back(ball);
	this->elements.push_back(ball);
	shared_ptr<Ball> ball1 = make_shared<Ball>(rend, 760, 760, 20, "test.png", 5, 1, -0.09, -0.09);
	this->balls.push_back(ball1);
	this->elements.push_back(ball1);
	shared_ptr<Ball> ball2 = make_shared<Ball>(rend, 200, 800, 30, "test.png", 5, 1, 0.2, -0.07);
	this->balls.push_back(ball2);
	this->elements.push_back(ball2);
	gen_combinations(this->combinations, this->balls.size());
	cout << combinations.size() << endl;
}

void Pool::physics(float time) {
	cout << endl << "------------" << endl << endl;
	cout << "Collision: " << endl;
	// Controllo collisione
	for (auto pair : this->combinations) {
		if (balls[pair.first]->is_colliding(balls[pair.second])) {
			cout << "Collision happened" << endl;
			balls[pair.first]->collide(balls[pair.second]);
		}
	}
	
	cout << endl << "Movement: " << endl;
	// Calcolo del movimento
	for (shared_ptr<Ball> &ball : this->balls)
		ball->movement(time);
}

void Pool::render(SDL_Renderer *rend) {
	SDL_SetRenderDrawColor(rend, 0, 128, 0, 1);
	SDL_RenderFillRect(rend, &this->pool);
	for (shared_ptr<Rendered> &element : this->elements) {
		element->render(rend);
	}
}

