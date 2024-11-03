#include <iostream>
#include <algorithm>

#include <SDL.h>
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

int main(int argc, char* argv[])
{
	srand(time(NULL)); 

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Eter-MC++", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		SCREEN_WIDTH,
		SCREEN_HEIGHT, 
		SDL_WINDOW_SHOWN);
	
	if (!window) {
		std::cout << "Window could not be created!" << std::endl
			<< "SDL_Error: " << SDL_GetError() << std::endl;
		return -1;
	}
	bool quit = false;
	SDL_Event e;
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();


	return 0;
}