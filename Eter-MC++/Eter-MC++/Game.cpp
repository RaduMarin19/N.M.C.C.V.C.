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
        static bool sendThisFrame = false;

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

            if (m_currentState == WELCOME_SCREEN && !sendThisFrame) {
                //Main Drawing area
                if (painter.drawLoginPage()) {
                    m_currentState = MODE_SELECTION;
                    sendThisFrame = true;
                }
            }

            if (m_currentState == MODE_SELECTION && !sendThisFrame) {
                painter.drawModeSelection();
                sendThisFrame = true;

                if (painter.isTrainingActive()) {
                    m_currentState = TRAINING_MODE;
                    board.generatePlayerCards(GameBoard::GameMode::Training);
                }
                else if (painter.isElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    board.generatePlayerCards(GameBoard::GameMode::Elemental);
                }
                else if (painter.isMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    board.generatePlayerCards(GameBoard::GameMode::MageDuel);
                }
                else if (painter.isTournamentActive())
                {
                    m_currentState = TOURNAMENT;
                    board.generatePlayerCards(GameBoard::GameMode::Tournament);
                }
                else if (painter.isQuickMatchActive())
                {
                    m_currentState = QUICK_MODE;
                    board.generatePlayerCards(GameBoard::GameMode::QuickMode);
                }
            }

            if (m_currentState == TRAINING_MODE) {
                for(const auto& card : board.getPlayerBlue()->GetCards())
                    painter.drawCard(card);
                for(const auto& card : board.getPlayerRed()->GetCards())
                    painter.drawCard(card);
            }
            

            // Update screen
            SDL_RenderPresent(painter.GetRenderer());
        }
        unsigned int frameTime = SDL_GetTicks() - frameStart;
        if(TARGET_FRAME_TIME > frameTime) {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
        sendThisFrame = false;
    }
}

