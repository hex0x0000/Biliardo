#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <exception>
#include <string>

class RenderingException : public std::exception {
	std::string msg;
	RenderingException(std::string msg) : msg(msg) {}
	~MyException() throw () {}
	const char* what() const throw() { return msg.c_str(); }
}

// Elemento che può essere renderizzato
class Rendered {
	public:
		Rendered(SDL_Renderer *rend, SDL_Rect rect, SDL_Surface *surface);
		Rendered(SDL_Renderer *rend, int x, int y, unsigned int radius, char *img_path);
		Rendered(SDL_Renderer *rend, SDL_Rect rect, char *img_path);
		void render(SDL_Renderer *rend);
		~Rendered();
	protected:
		SDL_Rect rect;
		SDL_Texture *texture;
}

// Palla da biliardo
class Ball : public Rendered {
	public:
		Ball(SDL_Renderer *rend, int x, int y, unsigned int radius, char *img_path) : Rendered(rend, x, y, radius, img_path);
		void calc_next();
	private:
		double vel_x;
		double vel_y;
		double angle;
		float friction;
		float mass;
		float ray;
}

// Muro
class Wall : public Rendered {
	public:
		Wall();
	
}

class Hole : public Rendered {
	public:
		Hole();
	// Texture
}

// Classe contenente tutti gli elementi del gioco
class Pool {
	public:
		Pool(SDL_Renderer *rend, unsigned int height, unsigned int width, unsigned short balls_num);
		// Calcola il frame successivo
		void next_frame();
		// Renderizza tutti gli elementi
		void render(SDL_Renderer *rend);
	private:
		// Vettori contenenti i puntatore a tutti gli elementi del gioco
		vector<std::shared_ptr<Rendered>> entities;
		vector<std::shared_ptr<Ball>> balls;
		vector<std::shared_ptr<Wall>> walls;
		vector<std::shared_ptr<Hole>> holes;
}

