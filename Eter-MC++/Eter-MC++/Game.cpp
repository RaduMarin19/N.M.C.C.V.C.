#include "Game.h"
#include "GameLogic.h"

Game::Game()
    : m_currentState(WELCOME_SCREEN) {
    srand(static_cast<unsigned int>(time(0)));
}


void Game::SetGameState(GameState state)
{
    m_currentState = state;
}

GameState Game::GetGameState() const
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
                    board.setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (painter.isElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    board.generatePlayerCards(GameBoard::GameMode::Elemental);
					board.setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (painter.isMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    board.generatePlayerCards(GameBoard::GameMode::MageDuel);
                    board.setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (g_config.tournamentActive)
                {
                    m_currentState = TOURNAMENT;
                    board.generatePlayerCards(GameBoard::GameMode::Tournament);
                    board.setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (painter.isQuickMatchActive())
                {
                    m_currentState = QUICK_MODE;
                    board.generatePlayerCards(GameBoard::GameMode::QuickMode);
                    board.setIsBluePlayer(true);
                    drawThisFrame = true;
                }
            }

            if (m_currentState == RED_PLAYER_WON || m_currentState == BLUE_PLAYER_WON || m_currentState == TIE) {
                std::string message = (m_currentState == RED_PLAYER_WON) ? "RED player WON!" :
                    (m_currentState == BLUE_PLAYER_WON) ? "Blue player WON!" : "TIE!";
                painter.drawText(message, { SCREEN_WIDTH / 2, 50 }, 14, true);

                bool isPressed = false;
                painter.drawButton(isPressed, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Return to menu!", 14);
                if (isPressed) {
                    m_currentState = MODE_SELECTION;
                    drawThisFrame = true;
                    painter.resetGameModes();
                    board.clear();
                }
            }
            

            if (m_currentState == TRAINING_MODE || m_currentState == ELEMENTAL_BATTLE ||
                m_currentState == MAGE_DUEL || m_currentState == TOURNAMENT || m_currentState == QUICK_MODE) {
                HandleBoardState(board, painter, m_currentState, drawThisFrame);
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

