#pragma once
#include <SDL2/SDL.h>
#include <memory>

// Elemento che può essere renderizzato
class Rendered {
	public:
		Rendered(SDL_Rect rect, char *img_path, SDL_Renderer *rend);
		void render(SDL_Renderer *rend);
		~Rendered();
	protected:
		SDL_Rect rect;
		SDL_Texture *texture;
}

// Palla da biliardo
class Ball : public Rendered {
	public:
		Ball(SDL_Rect rect, char *img_path, SDL_Renderer *rend) : Rendered(rect, img_path, rend);
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
		Pool();
		void calc_next();
		void render();
	private:
		vector<shared_ptr<Rendered>> entities;
}

