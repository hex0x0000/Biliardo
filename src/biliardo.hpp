#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_render.h>
#include <memory>
#include <exception>
#include <string>
#include <vector>

class RenderingException : public std::exception {
	public:
		std::string msg;
		RenderingException(std::string msg);
		//~RenderingException() throw ();
		const char* what() const throw();
};

// Elemento che può essere renderizzato
class Rendered {
	public:
		Rendered(SDL_Renderer *rend, SDL_Rect rect, SDL_Surface *surface);
		Rendered(SDL_Renderer *rend, int x, int y, int radius, std::string img_path);
		Rendered(SDL_Renderer *rend, SDL_Rect rect, std::string img_path);
		void render(SDL_Renderer *rend);
		~Rendered();
	protected:
		SDL_Rect rect;
		SDL_Texture *texture;
};

// Palla da biliardo
class Ball : public Rendered {
	public:
		Ball(SDL_Renderer *rend, int x, int y, int radius, std::string img_path, float friction, float mass);
		void calc_next();
	private:
		double vel_x;
		double vel_y;
		double angle;
		float friction;
		float mass;
		int radius;
};

// Muro
class Wall : public Rendered {
	public:
		Wall();
	
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
		void next_frame();
		// Renderizza tutti gli elementi
		void render(SDL_Renderer *rend);
	private:
		SDL_Rect pool;

		// Vettori contenenti i puntatore a tutti gli elementi del gioco
		std::vector<std::shared_ptr<Rendered>> elements;
		std::vector<std::shared_ptr<Ball>> balls;
		std::vector<std::shared_ptr<Wall>> walls;
		std::vector<std::shared_ptr<Hole>> holes;
};

