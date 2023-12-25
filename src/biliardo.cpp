#include "biliardo.hpp"
#include <SDL_image.h>
#include <cmath>

#include <cstdlib>
#include <ctime>

//#ifdef DEBUG
#include <iostream>

using std::cout;
using std::endl;
//#endif

using std::shared_ptr;
using std::make_shared;
using std::string;
using std::pair;
using std::set;
using std::make_pair;
using std::function;
using namespace std::literals::string_literals;

#define WALL_WIDTH 50

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
	: Rendered(rend, get_surface, x, y, radius*2, radius*2) {
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
	double vel_angle = atan2(vel_y, vel_x);
	double friction = this->friction * time * time;
	
	if (vel_x < -friction || vel_x > friction) {
		double x_friction = -friction * cos(vel_angle);
		vel_x += x_friction;
		cout << "\n\nFriction X: " << x_friction << "\n" << "vel_x: " << vel_x << "\n";
	} else {
		vel_x = 0;
		cout << "\n\nSet vel X to 0\n";
	}
	
	if (vel_y < -friction || vel_y > friction) {
		double y_friction = -friction * sin(vel_angle);
		vel_y += y_friction;
		cout << "Friction Y: " << y_friction << "\n" << "vel_y: " << vel_y << "\n" << "angle: " << vel_angle * (180 / 3.1415) << "\n";
	} else {
		vel_y = 0;
		cout << "Set vel Y to 0\n";
	}

	x += vel_x * time;
	y += vel_y * time;
	
	// Imposta la posizione del rettangolo che mostra
	// l'immagine della boccia
	rect.x = x - radius;
	rect.y = y - radius;
}

bool Ball::is_colliding(const shared_ptr<Ball> &ball) {
	double distance = sqrt(pow(ball->x - this->x, 2) + pow(ball->y - this->y, 2));
	if ((this->radius + ball->radius) > distance) {
		this->reset_coords();
		ball->reset_coords();
		return true;
	}
	return false;
}

void Ball::collide(std::shared_ptr<Ball> &ball) {
	int ball_x, ball_y;
	ball->get_coords(ball_x, ball_y);

	// Calcolo componenti normalizzate e tangenti al vettore normalizzato
	double unx, uny;
	{
		double modulo = sqrt(pow(ball->x + this->x, 2) + pow(ball->y + this->y, 2));
		unx = (this->x - ball_x)/modulo;
		uny = (this->y - ball_y)/modulo;
	}
	double utx = -uny;
	double uty = unx;
	double v1n = unx*this->vel_x+uny*this->vel_y;
	double v1t = utx*this->vel_x+uty*this->vel_y;
	double v2n = unx*ball->vel_x+uny*ball->vel_y;
	double v2t = utx*ball->vel_x+uty*ball->vel_y;

	// Calcolo dell'urto monodimensionale
	double v1nf, v2nf;
	{
		double masses = this->mass + ball->mass;	
		double delta_mass = this->mass - ball->mass;
		v1nf = ((v1n*delta_mass)+2*ball->mass*v2n)/masses;
		v2nf = ((v2n*(-delta_mass))+2*this->mass*v1n)/masses;
	}

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
	}
}

SDL_Surface *gen_wall_surface(int height, int width) {
	SDL_Surface *surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_LockSurface(surface);
	SDL_memset(surface->pixels, 0xD3BC8D, surface->h * surface->pitch);
	SDL_UnlockSurface(surface);
	return surface;
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
	
	int i = 0;
	std::srand(std::time(NULL));
	for (int x = 300; x < 800; x += 200) {
		for (int y = 300; y < 600; y += 200) {
			i++;
			double x_rand_vel = static_cast<double>(std::rand()) / RAND_MAX * 500;
			double y_rand_vel = static_cast<double>(std::rand()) / RAND_MAX * 500;
			shared_ptr<Ball> ball = make_shared<Ball>(rend, 
				[](string &_) -> SDL_Surface* { return IMG_Load("test.png"); },
				x, y, 30, 5, 50.0, x_rand_vel, y_rand_vel);
			this->balls.push_back(ball);
			this->elements.push_back(ball);
			cout << "NEW " << i << ": " << endl;
			cout << "X: " << x << endl;
			cout << "Y: " << y << endl;
			cout << "X_RAND_VEL: " << x_rand_vel << endl;
			cout << "Y_RAND_VEL: " << y_rand_vel << endl << endl;
		}
	}
	
	// RIGHT
	shared_ptr<Wall> wall1 = make_shared<Wall>(rend,
			[height](string &_) -> SDL_Surface* { return gen_wall_surface(height, WALL_WIDTH); },
			width - WALL_WIDTH, this->pool.y, height, WALL_WIDTH, -1, 1);
	this->walls.push_back(wall1);
	
	// BOTTOM
	shared_ptr<Wall> wall2 = make_shared<Wall>(rend,
			[width](string &_) -> SDL_Surface* { return gen_wall_surface(WALL_WIDTH, width); },
			0, height - WALL_WIDTH, WALL_WIDTH, width, 0.999, -0.999);
	this->walls.push_back(wall2);
	
	// LEFT
	shared_ptr<Wall> wall3 = make_shared<Wall>(rend,
			[height](string &_) -> SDL_Surface* { return gen_wall_surface(height, WALL_WIDTH); },
			0, this->pool.y, height, WALL_WIDTH, -0.1, 0.1);
	this->walls.push_back(wall3);
	
	// TOP
	shared_ptr<Wall> wall4 = make_shared<Wall>(rend,
			[width](string &_) -> SDL_Surface* { return gen_wall_surface(WALL_WIDTH, width); },
			0, this->pool.y, WALL_WIDTH, width, 0.999, -0.999);
	this->walls.push_back(wall4);
	for (auto wall : this->walls)
		this->elements.push_back(wall);
	//this->elements.push_back(wall);
	/*shared_ptr<Ball> ball1 = make_shared<Ball>(rend, 760, 760, 20, "test.png", 0.009, 1, -0.9, -0.9);
	this->balls.push_back(ball1);
	this->elements.push_back(ball1);
	shared_ptr<Ball> ball2 = make_shared<Ball>(rend, 200, 800, 30, "test.png", 0.009, 1.4, 0.2, -0.9);
	this->balls.push_back(ball2);
	this->elements.push_back(ball2);*/
	gen_combinations(this->combinations, this->balls.size());
}

void Pool::physics(float time) {
	// Controllo collisioni
	for (auto pair : this->combinations) {
		if (balls[pair.first]->is_colliding(balls[pair.second])) {
			cout << "Collision happened: " << pair.first << " <-> " << pair.second << endl;
			balls[pair.first]->collide(balls[pair.second]);
		}
	}
	
	for (auto &ball : balls)
		for (auto &wall : walls) 
			wall->collide(ball);

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

