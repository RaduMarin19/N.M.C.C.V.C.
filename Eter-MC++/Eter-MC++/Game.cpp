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

    //Initialize graphics object to handle drawing
    Graphics painter;

    //Initialize a board object and pass it the rendering context
    GameBoard board(painter.GetRenderer());

    bool quit = false;
    SDL_Event e;


    while (!quit)
    {
        //Get start time of current frame
        unsigned int frameStart = SDL_GetTicks();

        //We poll all events in the list, use this variable to only draw the next menu on the next frame, not the next event
        static bool drawThisFrame = false;

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

            if (m_currentState == WELCOME_SCREEN && !drawThisFrame) {

                //Draw the login page until user moves to the next page
                if (painter.drawLoginPage()) {
                    m_currentState = MODE_SELECTION;
                    drawThisFrame = true;
                }
            }

            if (m_currentState == MODE_SELECTION && !drawThisFrame) {
                //Draw the mode selection menu
                painter.drawModeSelection();

                //Check which button the player has pressed, and initialize the cards accordingly
                if (painter.isTrainingActive()) {
                    m_currentState = TRAINING_MODE;
                    board.generatePlayerCards(GameBoard::GameMode::Training);
                    drawThisFrame = true;
                }
                else if (painter.isElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    board.generatePlayerCards(GameBoard::GameMode::Elemental);
                    drawThisFrame = true;
                }
                else if (painter.isMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    board.generatePlayerCards(GameBoard::GameMode::MageDuel);
                    drawThisFrame = true;
                }
                else if (painter.isTournamentActive())
                {
                    m_currentState = TOURNAMENT;
                    board.generatePlayerCards(GameBoard::GameMode::Tournament);
                    drawThisFrame = true;
                }
                else if (painter.isQuickMatchActive())
                {
                    m_currentState = QUICK_MODE;
                    board.generatePlayerCards(GameBoard::GameMode::QuickMode);
                    drawThisFrame = true;
                }
            }


            if (m_currentState == TRAINING_MODE) {
                //Iterate each players' cards and draw them onto the screen
                //This is where all the in game logic will go
                for(const auto& card : board.getPlayerBlue()->GetCards())
                    painter.drawCard(card);
                for(const auto& card : board.getPlayerRed()->GetCards())
                    painter.drawCard(card);
            }
            

            // Update screen
            SDL_RenderPresent(painter.GetRenderer());
        }
        //Get total elapsed time since we started to draw the frame, if less than the target frame time, sleep
        unsigned int frameTime = SDL_GetTicks() - frameStart;
        if(TARGET_FRAME_TIME > frameTime) {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
        drawThisFrame = false;
    }
}

