#include "Game.h"

Game::Game()
    : m_currentState(WELCOME_SCREEN),
    m_window(nullptr),
    m_renderer(nullptr)
{
    srand(static_cast<unsigned int>(time(NULL)));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL Initialization Failed");
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
    // Disable compositor bypass
    if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
        std::cerr << "SDL cannot disable compositor bypass!" << std::endl;
        throw std::runtime_error("SDL Hint Setting Failed");
    }
#endif

    // Create window
    m_window = SDL_CreateWindow("Basic C++ SDL project",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (!m_window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL Window Creation Failed");
    }

    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL Renderer Creation Failed");
    }

    // Initialize TTF
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init Failed! TTF_Error: " << TTF_GetError() << std::endl;
        throw std::runtime_error("TTF Initialization Failed");
    }
}

Game::~Game()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    TTF_Quit();
    SDL_Quit();
}

void Game::SetGameState(Game::GameState state)
{
    m_currentState = state;
}

Game::GameState Game::GetGameState() const
{
    return m_currentState;
}

void Game::run() {
    Graphics painter(m_renderer);

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
            SDL_SetRenderDrawColor(m_renderer, 0x0f, 0x0f, 0x0f, 0xFF);

            // Clear screen
            SDL_RenderClear(m_renderer);

            if (m_currentState == WELCOME_SCREEN) {
                //Main Drawing area
                if (painter.drawLoginPage()) {
                    m_currentState = MODE_SELECTION;
                }
            }

            if (m_currentState == MODE_SELECTION) {
                painter.drawModeSelection();
                if (painter.isTrainingActive()) {
                    m_currentState = TRAINING_MODE;
                }
            }

            if (m_currentState == TRAINING_MODE) {

            }
            

            // Update screen
            SDL_RenderPresent(m_renderer);
        }
        unsigned int frameTime = SDL_GetTicks() - frameStart;
        if(TARGET_FRAME_TIME > frameTime) {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
    }
}

