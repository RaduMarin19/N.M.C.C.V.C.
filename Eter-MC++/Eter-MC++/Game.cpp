#include "Game.h"

Game::Game()
    : m_currentState(WELCOME_SCREEN) {}


void Game::SetGameState(Game::GameState state)
{
    m_currentState = state;
}

Game::GameState Game::GetGameState() const
{
    return m_currentState;
}

void Game::run() {
    Graphics painter;
    GameBoard board(painter.GetRenderer());

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
            SDL_SetRenderDrawColor(painter.GetRenderer(), 0x0f, 0x0f, 0x0f, 0xFF);

            // Clear screen
            SDL_RenderClear(painter.GetRenderer());

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
                painter.drawCard(board.getPlayerBlue()->GetCards()[1]);
            }
            

            // Update screen
            SDL_RenderPresent(painter.GetRenderer());
        }
        unsigned int frameTime = SDL_GetTicks() - frameStart;
        if(TARGET_FRAME_TIME > frameTime) {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
    }
}

