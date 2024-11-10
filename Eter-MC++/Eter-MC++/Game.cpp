#include "Game.h"

Game::Game()
    : m_currentState(WELCOME_SCREEN) {}


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
            if (m_currentState == RED_PLAYER_WON)
            {
                std::cout << "Red Player Won";
                //m_currentState = MODE_SELECTION;
            }
                
            if (m_currentState == BLUE_PLAYER_WON) 
            {
                std::cout << "Blue Player Won";
                //m_currentState = MODE_SELECTION;
            }

            if (m_currentState == TRAINING_MODE) {

                if (board.getPlayerBlue()->HasPlayedIllusion() == false&& board.isBluePlayer()) {
                    painter.drawButton(board.getPlayerBlue()->isPlayingIllusion(), {SCREEN_WIDTH - 200, SCREEN_HEIGHT - 140}, 100, 40, "Play illusion!", 14);
                }

                if (board.getPlayerRed()->HasPlayedIllusion() == false&&!board.isBluePlayer()) {
                    painter.drawButton(board.getPlayerRed()->isPlayingIllusion(), { SCREEN_WIDTH - 200, SCREEN_HEIGHT - 140 }, 100, 40, "Play illusion!", 14);
               
                }
                //Draw the board, with the possible positions and played cards;
                for(const auto& possiblePosition : board.GetPossiblePositions()) {
                    SDL_Rect renderRect;
                    renderRect.x = SCREEN_WIDTH / 2 - textureWidth / 2 - (possiblePosition.GetX() * textureWidth);
                    renderRect.y =  SCREEN_HEIGHT / 2 - textureHeight / 2 - (possiblePosition.GetY() * textureHeight);
                    renderRect.w = textureWidth;
                    renderRect.h = textureHeight;
                    
                    if(painter.isMouseInRect(renderRect)) {
                        SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 50, 255);
                       
                        if(board.isBluePlayer() &&board.getPlayerBlue()->isGrabbingCard() && !painter.isPressingLeftClick()) {
                            PlayingCard pushCard = *board.getPlayerBlue()->GetGrabbedCard();
                            pushCard.SetBoardPosition(possiblePosition);
                            pushCard.SetCoordinates({renderRect.x, renderRect.y});

                            //should move the illusion checking after pushing the card for both players
                            if (board.pushNewCard(pushCard)) {
                                board.getPlayerBlue()->removeCard(*board.getPlayerBlue()->GetGrabbedCard());
                                board.setIsBluePlayer(false);
                                if (board.getPlayerBlue()->HasPlayedIllusion() == false && board.getPlayerBlue()->isPlayingIllusion())
                                {   
                                    for(auto& card: board.GetPlayedPositions())
                                        if(card.second.top() == pushCard)
                                            card.second.top().SetIllussion(board.getPlayerBlue()->isPlayingIllusion());
                                    board.getPlayerBlue()->SetHasPlayedIllusion();
                                }
                                board.checkStatus(m_currentState); //still needs work
                            }
                            else {

                            }
                           
                        }
                        else if (board.getPlayerRed()->isGrabbingCard() && !painter.isPressingLeftClick()) {
                            PlayingCard pushCard = *board.getPlayerRed()->GetGrabbedCard();
                            pushCard.SetBoardPosition(possiblePosition);
                            pushCard.SetCoordinates({ renderRect.x, renderRect.y });

                            if (board.pushNewCard(pushCard)) {
                                board.getPlayerRed()->removeCard(*board.getPlayerRed()->GetGrabbedCard());
                                if (board.getPlayerRed()->HasPlayedIllusion() == false && board.getPlayerRed()->isPlayingIllusion()) {
                                    for (auto& card : board.GetPlayedPositions())
                                        if (card.second.top() == pushCard)
                                            card.second.top().SetIllussion(board.getPlayerRed()->isPlayingIllusion());
                                            board.getPlayerRed()->SetHasPlayedIllusion();
                                }
                                board.setIsBluePlayer(true);
                                board.checkStatus(m_currentState);
                            }
                        }
                    } else {
                        SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 255, 255);
                    }
                    SDL_RenderDrawRect(painter.GetRenderer(), &renderRect);

                }

                for(const auto& cards = board.GetPlayedCards(); const auto& card : cards) {
                    if (card.isIllusion())
                    {
                        if (card.GetColor() == BLUE)
                            painter.drawCard(card, board.getBlueIllusionTexture()->getTexture());
                        else if(card.GetColor() == RED)
                        {
                            painter.drawCard(card, board.getRedIllusionTexture()->getTexture());
                        }
                    }
                    else
                    {
                        painter.drawCard(card, card.GetTexture()->getTexture());
                    }
                }

                //Iterate each players' cards and draw them onto the screen
                //This is where all the in game logic will go
                for(auto& card : board.getPlayerBlue()->GetCards()) {
                    SDL_Rect cardRect;
                    cardRect.x = card.GetCoordinates().GetX();
                    cardRect.y = card.GetCoordinates().GetY();
                    cardRect.w = textureWidth;
                    cardRect.h = textureHeight;
                    if(board.isBluePlayer()) {
                        painter.drawCard(card, card.GetTexture()->getTexture());
                        if(painter.isMouseInRect(cardRect)) {
                            SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 50, 255);
                            SDL_RenderDrawRect(painter.GetRenderer(), &cardRect);
                            if(painter.isPressingLeftClick() && !board.getPlayerBlue()->isGrabbingCard()) {
                                board.getPlayerBlue()->SetIsGrabbingCard(true);
                                board.getPlayerBlue()->SetGrabbedCard(&card);
                            }
                            if(board.getPlayerBlue()->isGrabbingCard()) {
                                if(board.getPlayerBlue()->GetGrabbedCard()->GetId() == card.GetId()) {
                                    //std::cout << "Player blue is grabbing a card\n";
                                    Coordinates mousePos = painter.getMousePos();
                                    mousePos.SetX(mousePos.GetX() - (textureWidth / 2));
                                    mousePos.SetY(mousePos.GetY() - (textureHeight / 2));
                                    card.SetCoordinates(mousePos);
                                }
                            }
                        }
                    } else {
                        painter.drawCard(card, board.getBlueIllusionTexture()->getTexture());
                    }
                }
                for(auto& card : board.getPlayerRed()->GetCards()) {
                    SDL_Rect cardRect;
                    cardRect.x = card.GetCoordinates().GetX();
                    cardRect.y = card.GetCoordinates().GetY();
                    cardRect.w = textureWidth;
                    cardRect.h = textureHeight;
                    if(!board.isBluePlayer()) {
                        painter.drawCard(card, card.GetTexture()->getTexture());
                        if (painter.isMouseInRect(cardRect)) {
                            SDL_SetRenderDrawColor(painter.GetRenderer(), 250, 250, 50, 255);
                            SDL_RenderDrawRect(painter.GetRenderer(), &cardRect);

                            if (painter.isPressingLeftClick() && !board.getPlayerRed()->isGrabbingCard()) {
                                board.getPlayerRed()->SetIsGrabbingCard(true);
                                board.getPlayerRed()->SetGrabbedCard(&card);
                            }
                            if (board.getPlayerRed()->isGrabbingCard()) {
                                if (board.getPlayerRed()->GetGrabbedCard()->GetId() == card.GetId()) {
                                    //std::cout << "Player red is grabbing a card\n";
                                    Coordinates mousePos = painter.getMousePos();
                                    mousePos.SetX(mousePos.GetX() - (textureWidth / 2));
                                    mousePos.SetY(mousePos.GetY() - (textureHeight / 2));
                                    card.SetCoordinates(mousePos);
                                }
                            }
                        }
                    } else {
                        painter.drawCard(card, board.getRedIllusionTexture()->getTexture());
                    }
                }
                if(!painter.isPressingLeftClick() && (board.getPlayerRed()->isGrabbingCard() || board.getPlayerBlue()->isGrabbingCard()) ) {
                    //std::cout << "Player stopped grabbing a card\n";
                    board.getPlayerRed()->SetIsGrabbingCard(false);
                    board.getPlayerBlue()->SetIsGrabbingCard(false);
                }
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

