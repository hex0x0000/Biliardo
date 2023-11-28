#include "biliardo.hpp"
#include <SDL_image.h>
#include <cmath>

#ifdef DEBUG
#include <iostream>

using std::cout;
using std::endl;
#endif

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::set;
using std::make_pair;
using std::function;
using namespace std::literals::string_literals;

RenderingException::RenderingException(string msg) {
	this->msg = msg;
}

const char *RenderingException::what() const throw() {
	return msg.c_str();
}

Rendered::Rendered(SDL_Renderer *rend, function<SDL_Surface*(string&)> get_surface, int x, int y, int h, int w) {
	this->rect = {.x = x, .y = y, .w = w, .h = h};
	string err_msg("");
	SDL_Surface *surface = get_surface(err_msg);
	if (!surface)
		throw RenderingException("Impossibile caricare immagine: "s + err_msg + "\nSDL: "s + string(SDL_GetError()));
	this->texture = SDL_CreateTextureFromSurface(rend, surface);
	if (!this->texture)
		throw RenderingException("Impossibile creare texture: "s + string(SDL_GetError()));
	SDL_FreeSurface(surface);
}

void Rendered::render(SDL_Renderer *rend) {
	int e = SDL_RenderCopy(rend, this->texture, NULL, &this->rect);
	if (e != 0)
		throw RenderingException("Rendering texture fallito: "s + string(SDL_GetError()));
}

bool Rendered::has_intersection(SDL_Rect *rect) {
	return (bool)SDL_HasIntersection(&this->rect, rect);
}

Rendered::~Rendered() {
	SDL_DestroyTexture(this->texture);
}

Ball::Ball(SDL_Renderer *rend, function<SDL_Surface*(string&)> get_surface, int x, int y, int radius, float friction, float mass, double vel_x, double vel_y)
	: Rendered(rend, get_surface, x, y, radius, radius) {
	this->vel_x = vel_x;
	this->vel_y = vel_y;
	this->friction = friction / radius;
	this->mass = mass;
	this->radius = radius;
	this->x = x;
	this->y = y;
	this->rect.x -= radius;
	this->rect.y -= radius;
}

void Ball::get_coords(int &x, int &y) {
	x = this->x;
	y = this->y;
}

void Ball::reset_coords() {
	x = before_x;
	y = before_y;
}

void Ball::movement(float time) {
	before_x = x;
	before_y = y;
	double delta_x = vel_x * time;
	double delta_y = vel_y * time;
	double friction = this->friction * time;
	if (delta_x != 0.0) {
		x += delta_x;
		if (abs(vel_x) >= friction)
			vel_x += ((vel_x > 0) ? -1 : 1) * friction;
		else
			vel_x = 0;
	}
	
	if (delta_y != 0.0) {
		y += delta_y;
		if (abs(vel_y) >= friction)
			vel_y += ((vel_y > 0) ? -1 : 1) * friction;
		else
			vel_y = 0;
	}

	// Imposta la posizione del rettangolo che mostra
	// l'immagine della boccia
	rect.x = x - radius;
	rect.y = y - radius;

#ifdef DEBUG
	cout << "x: " << x << endl;
	cout << "y: " << y << endl;
	cout << "vel x: " << this->vel_x << endl;
	cout << "vel y: " << this->vel_y << endl;
	cout << "delta x: " << delta_x << endl;
	cout << "delta y: " << delta_y << endl << endl;
#endif
}

bool Ball::is_colliding(const shared_ptr<Ball> &ball) {
	double distance = sqrt(pow(ball->x - x, 2) + pow(ball->y - y, 2));
#ifdef DEBUG
	cout << "distance: " << distance << endl;
	cout << "radius sum: " << ball->radius + radius << endl;
#endif
	if ((int)(radius + ball->radius) > distance) {
		reset_coords();
		ball->reset_coords();
		return true;
	}
	return false;
}

void Ball::collide(std::shared_ptr<Ball> &ball) {
	int ball_x, ball_y;
	ball->get_coords(ball_x, ball_y);

	// Calcolo componenti normalizzate e tangenti al vettore normalizzato
	double modulo = sqrt(pow(ball->x + this->x, 2) + pow(ball->y + this->y, 2));
	double unx = (this->x - ball_x)/modulo;
	double uny = (this->y - ball_y)/modulo;
	double utx = -uny;
	double uty = unx;
	double v1n = unx*this->vel_x+uny*this->vel_y;
	double v1t = utx*this->vel_x+uty*this->vel_y;
	double v2n = unx*ball->vel_x+uny*ball->vel_y;
	double v2t = utx*ball->vel_x+uty*ball->vel_y;

	// Calcolo dell'urto monodimensionale
	double masses = this->mass + ball->mass;	
	double delta_mass = this->mass - ball->mass;
	double v1nf = ((v1n*delta_mass)+2*ball->mass*v2n)/masses;
	double v2nf = ((v2n*(-delta_mass))+2*this->mass*v1n)/masses;

	// Calcolo delle velocità nelle due dimensioni
	this->vel_x = v1nf*unx+v1t*utx;
	this->vel_y = v1nf*uny+v1t*uty;
	ball->vel_x = v2nf*unx+v2t*utx;
	ball->vel_y = v2nf*uny+v2t*uty;
}

Wall::Wall(SDL_Renderer *rend, function<SDL_Surface*(string&)> get_surface, int x, int y, int h, int w, short x_turn, short y_turn)
	: Rendered(rend, get_surface, x, y, h, w) {
	this->x_turn = x_turn;
	this->y_turn = y_turn;
}

void Wall::collide(std::shared_ptr<Ball> &ball) {
	if (ball->has_intersection(&rect)) {
		ball->vel_x *= x_turn;
		ball->vel_y *= y_turn;
		ball->reset_coords();
#ifdef DEBUG
		cout << "Collision with wall happened" << endl;
#endif
	}
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
	this->pool = {.x = 0, .y = 70, .w = width, .h = height};
	/*for (int i = 0; i < balls_num; i++) {
		shared_ptr<Ball> ball = make_shared<Ball>(rend, 500, 500, 75, "test.jpg", 0.3, 1);
		this->balls.push_back(ball);
		this->elements.push_back(ball);
	}*/
	
	shared_ptr<Ball> ball = make_shared<Ball>(rend, 
			[](string &_) -> SDL_Surface* { return IMG_Load("test.png"); },
			700, 100, 40, 0.009, 2.5, 0.56, 0.61);
	this->balls.push_back(ball);
	this->elements.push_back(ball);
	shared_ptr<Wall> wall = make_shared<Wall>(rend,
			[height](string &_) -> SDL_Surface* {
				SDL_Surface *surface = SDL_CreateRGBSurface(0, 50, height, 32, 0, 0, 0, 0);
				SDL_LockSurface(surface);
				SDL_memset(surface->pixels, 0xD3BC8D, surface->h * surface->pitch);
				SDL_UnlockSurface(surface);
				return surface;
			},
			width - 50, 100, height, 50, -1, 1);
	this->walls.push_back(wall);
	this->elements.push_back(wall);
	/*shared_ptr<Ball> ball1 = make_shared<Ball>(rend, 760, 760, 20, "test.png", 0.009, 1, -0.9, -0.9);
	this->balls.push_back(ball1);
	this->elements.push_back(ball1);
	shared_ptr<Ball> ball2 = make_shared<Ball>(rend, 200, 800, 30, "test.png", 0.009, 1.4, 0.2, -0.9);
	this->balls.push_back(ball2);
	this->elements.push_back(ball2);*/
	gen_combinations(this->combinations, this->balls.size());
}

void Pool::physics(float time) {
#ifdef DEBUG
	cout << endl << "------------" << endl << endl;
	cout << "Collision: " << endl;
#endif
	// Controllo collisioni
	for (auto pair : this->combinations) {
		if (balls[pair.first]->is_colliding(balls[pair.second])) {
#ifdef DEBUG
			cout << "Collision happened" << endl;
#endif
			balls[pair.first]->collide(balls[pair.second]);
		}
	}
	
	for (auto &ball : balls)
		for (auto &wall : walls) 
			wall->collide(ball);

#ifdef DEBUG	
	cout << endl << "Movement: " << endl;
#endif
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

