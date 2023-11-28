#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>
#include <memory>
#include <exception>
#include <string>
#include <vector>
#include <utility>
#include <set>
#include <functional>

class RenderingException : public std::exception {
	public:
		std::string msg;
		RenderingException(std::string msg);
		const char* what() const throw();
};

// Elemento che può essere renderizzato
class Rendered {
	public:
		Rendered(SDL_Renderer *rend, std::function<SDL_Surface*(std::string&)> get_surface, int x, int y, int h, int w);
		void render(SDL_Renderer *rend);
		bool has_intersection(SDL_Rect *rect);
		~Rendered();
	protected:
		SDL_Rect rect;
		SDL_Texture *texture;
};

// Palla da biliardo
class Ball : public Rendered {
	public:
		Ball(SDL_Renderer *rend, std::function<SDL_Surface*(std::string&)> get_surface, int x, int y, int radius, float friction, float mass = 1.0, double vel_x = 0.0, double vel_y = 0.0);
		void get_coords(int &x, int &y);
		void reset_coords();
		// Calcolo movimento ogni frame
		void movement(float time);
		// Controlla se c'è una collisione
		bool is_colliding(const std::shared_ptr<Ball> &ball);
		// Calcola la collisione
		void collide(std::shared_ptr<Ball> &ball);
		double vel_x;
		double vel_y;
	private:
		int x;
		int y;
		int before_x;
		int before_y;
		float friction;
		float mass;
		int radius;
};

// Muro
class Wall : public Rendered {
	public:
		Wall(SDL_Renderer *rend, std::function<SDL_Surface*(std::string&)> get_surface, int x, int y, int h, int w, short x_turn = 1, short y_turn = 1);
		void collide(std::shared_ptr<Ball> &ball);
	private:
		float x_turn;
		float y_turn;
	
};

class Hole : public Rendered {
	public:
		Hole();
	// Texture
};

// Classe contenente tutti gli elementi del gioco
class Pool {
	public:
		Pool(SDL_Renderer *rend, int height, int width, int balls_num);
		// Calcola il frame successivo
		void physics(float time);
		// Renderizza tutti gli elementi
		void render(SDL_Renderer *rend);
	private:
		SDL_Rect pool;
		std::set<std::pair<int, int>> combinations;

		// Vettori contenenti i puntatore a tutti gli elementi del gioco
		std::vector<std::shared_ptr<Rendered>> elements;
		std::vector<std::shared_ptr<Ball>> balls;
		std::vector<std::shared_ptr<Wall>> walls;
		std::vector<std::shared_ptr<Hole>> holes;
};

