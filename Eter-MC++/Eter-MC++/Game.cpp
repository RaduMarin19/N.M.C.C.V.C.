#include "Game.h"

Game::Game() {

}

void Game::run() {

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if(!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0"))
    {
        std::cout << "SDL can not disable compositor bypass!" << std::endl;
        return;
    }
#endif

    // Create window
    SDL_Window *window = SDL_CreateWindow("Basic C++ SDL project",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if(!window) {
        std::cout << "Window could not be created!" << std::endl
                << "SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer)
    {
        std::cout << "Renderer could not be created!" << std::endl
                << "SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }


    Graphics painter(renderer);

    bool quit = false;
    SDL_Event e;
    while (!quit)
    {
        unsigned int frameStart = SDL_GetTicks();
        while (SDL_PollEvent(&e) != 0)
        {
            painter.setEvent(e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            // Initialize renderer color white for the background
            SDL_SetRenderDrawColor(renderer, 0x0f, 0x0f, 0x0f, 0xFF);

            // Clear screen
            SDL_RenderClear(renderer);

            //Main Drawing area
            painter.draw();

            // Update screen
            SDL_RenderPresent(renderer);
        }
        unsigned int frameTime = SDL_GetTicks() - frameStart;
        if(TARGET_FRAME_TIME > frameTime) {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

