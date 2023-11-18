#include <SDL2/SDL.h>
#include <SDL_image.h>
#include <thread>
#include <chrono>
#include <string>
#include <iostream>

#include "biliardo.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::stoi;

using namespace std::chrono_literals;

int main(int argc, char *argv[]) {	
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        cerr << "Impossibile inizializzare SDL: " << SDL_GetError() << endl;
    // Parsing degli argomenti dati dal terminale per prendere la grandeza della finestra
	int height, width;
	if (argc > 2) {
		height = stoi(argv[1]);
		width = stoi(argv[2]);
		if (height <= 600 || width <= 300) {
			cerr << "Grandezza invalida" << endl;
			return 1;
		}
	} else {
		height = 900;
		width = 1200;
	}

	// Creazione finestra
	SDL_Window* win = SDL_CreateWindow("Biliardo",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       width, height, 0);
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

	// Main del gioco
	{
		bool run = true;

		// Creazione del pool di biliardino
		Pool pool(rend, height, width, 1);

		while (run) {
			SDL_Event event;
			
			// Ricevimento input (tastiera, mouse...)
			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT:
						run = false;
						break;
					default:
						break;
				}
			}
			// Pulisce lo schermo dal frame precedente
			SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
			SDL_RenderClear(rend);

			// Renderizza gli elementi del gioco
			pool.render(rend);

			// Mostra gli elementi e aspetta per fare 64 fps
			SDL_RenderPresent(rend);
			std::this_thread::sleep_for(16.667ms);
		}
	}
    
	// Chiusura gioco
	SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
	return 0;
}
 
