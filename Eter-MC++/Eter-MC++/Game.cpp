#include "Game.h"

Game::Game()
    : m_currentState(WELCOME_SCREEN) {
    m_painter = std::make_shared<Graphics>();
    m_board = std::make_unique<GameBoard>(m_painter.get()->GetRenderer());
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
            
            m_painter->setEvent(e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }

            // Initialize renderer color white for the background
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 0x0f, 0x0f, 0x0f, 0xFF);

            // Clear screen
            SDL_RenderClear(m_painter->GetRenderer());

            if (m_currentState == WELCOME_SCREEN && !drawThisFrame) {

                //Draw the login page until user moves to the next page
                if (m_painter->drawLoginPage()) {
                    m_currentState = MODE_SELECTION;
                    drawThisFrame = true;
                }
            }

            if (m_currentState == MODE_SELECTION && !drawThisFrame) {

                //Draw the mode selection menu
                m_painter->drawModeSelection();

                //Check which button the player has pressed, and initialize the cards accordingly
                if (m_painter->isTrainingActive()) {
                    m_currentState = TRAINING_MODE;
                    m_board->generatePlayerCards(GameBoard::GameMode::Training);
                    m_board->setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (m_painter->isElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    m_board->setGameMode(GameBoard::GameMode::Elemental);
                    m_board->generatePlayerCards(GameBoard::GameMode::Elemental);
					m_board->setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (m_painter->isMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    m_board->setGameMode(GameBoard::GameMode::MageDuel);
                    m_board->generatePlayerCards(GameBoard::GameMode::MageDuel);
                    m_board->setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (m_painter->isTournamentActive())
                {
                    m_currentState = TOURNAMENT;
					drawThisFrame = true;
                }
                else if (m_painter->isQuickMatchActive())
                {
                    m_currentState = QUICK_MODE;
					m_board->setGameMode(GameBoard::GameMode::QuickMode);
                    m_board->generatePlayerCards(GameBoard::GameMode::QuickMode);
                    m_board->setIsBluePlayer(true);
                    drawThisFrame = true;
                }
            }

            if (m_currentState == TOURNAMENT && !drawThisFrame)
            {
                m_painter->drawTournamentModeSelection();
                if (m_painter->isElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    m_board->setGameMode(GameBoard::GameMode::Elemental);
                    m_board->generatePlayerCards(GameBoard::GameMode::Elemental);
					m_board->setIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (m_painter->isMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    m_board->setGameMode(GameBoard::GameMode::MageDuel);
                    m_board->generatePlayerCards(GameBoard::GameMode::MageDuel);
                    m_board->setIsBluePlayer(true);
                    drawThisFrame = true;
                }
            }

            if (m_currentState == RED_PLAYER_WON || m_currentState == BLUE_PLAYER_WON || m_currentState == TIE) {
                std::string message = (m_currentState == RED_PLAYER_WON) ? "RED player WON!" :
                    (m_currentState == BLUE_PLAYER_WON) ? "Blue player WON!" : "TIE!";
                m_painter->drawText(message, { SCREEN_WIDTH / 2, 50 }, 14, true);

                bool isPressed = false;
                m_painter->drawButton(isPressed, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Return to menu!", 14);
                if (isPressed) {
                    m_currentState = MODE_SELECTION;
                    drawThisFrame = true;
                    m_painter->resetGameModes();
                    m_board->clear();
                }
            }
            

            if (m_currentState == TRAINING_MODE || m_currentState == ELEMENTAL_BATTLE ||
                m_currentState == MAGE_DUEL || m_currentState == TOURNAMENT || m_currentState == QUICK_MODE) {
                HandleBoardState();
            }

            // Update screen
            SDL_RenderPresent(m_painter->GetRenderer());
        }
        //Get total elapsed time since we started to draw the frame, if less than the target frame time, sleep
        unsigned int frameTime = SDL_GetTicks() - frameStart;
        if(TARGET_FRAME_TIME > frameTime) {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
        drawThisFrame = false;
    }
}

void Game::HandleBoardState() {

    if (m_currentState == TOURNAMENT)
        return;

    if (m_currentState == TRAINING_MODE || m_currentState == QUICK_MODE)
        m_board->setTable(3);
    else
        m_board->setTable(4);

    m_board->updateBoardCenter();
    if (m_board->getExplosion() == nullptr) {
        std::cout << "generating expl\n";
        m_board->initializeExplosion();
    }

    if (m_currentState == TRAINING_MODE || m_currentState == MAGE_DUEL || m_currentState == ELEMENTAL_BATTLE)
    {
        if (m_board->canUseExplosion() && m_board->didExplode() == false) {
            static bool checkExplosion = false;

            {
                SDL_Rect explosionRect{ SCREEN_WIDTH / 2 - textureWidth * 3, SCREEN_HEIGHT - 500, 128, 128 };
                m_painter->drawTexturedRect(explosionRect, m_board->GetExplosionBoardTexture()->getTexture());
                auto explosion = m_board->getExplosion();
                decltype(auto) explosionMask = explosion->GetExplosionMask();
                for (int i = 0; i < m_board->getTableSize(); i++) {
                    for (int j = 0; j < m_board->getTableSize(); j++) {
                        SDL_Rect spriteRect{ explosionRect.x + 12 + (i * 34), explosionRect.y + 6 + (j * 32), 32, 32 };
                        if (explosionMask[i][j] == ExplosionType::HOLE) {
                            m_painter->drawTexturedRect(spriteRect, m_board->GetExplosionSprite(2)->getTexture());
                        }
                        else if (explosionMask[i][j] == ExplosionType::DELETE) {
                            m_painter->drawTexturedRect(spriteRect, m_board->GetExplosionSprite(0)->getTexture());
                        }
                        else if (explosionMask[i][j] == ExplosionType::RETURN) {
                            m_painter->drawTexturedRect(spriteRect, m_board->GetExplosionSprite(1)->getTexture());
                        }
                    }

                }
            }

            if (!checkExplosion) {
                m_painter->drawButton(checkExplosion, { SCREEN_WIDTH - 1000, SCREEN_HEIGHT - 100 }, 120, 50, "Check explosion!", 14);
            }
            if (checkExplosion) {
                m_drawThisFrame = false;
                bool exploded = false;
                m_painter->drawButton(exploded, { SCREEN_WIDTH - 750, SCREEN_HEIGHT - 100 }, 100, 50, "Explode!", 14);
                bool rotate = false;
                m_painter->drawButton(rotate, { SCREEN_WIDTH - 550, SCREEN_HEIGHT - 100 }, 100, 50, "Rotate!", 14);
                if (exploded) {
                    //if (board.validateExplosion())
                    m_board->explode();
                    //else
                    //   std::cout << "Explosion invalidates map!\n";
                }
                if (rotate) {
                    m_board->getExplosion()->rotateExplosion();
                    std::cout << "----------------\n";
                    m_board->printExplosionMask();
                }
            }
        }
    }

    // Common logic for all modes

    if (m_board->getPlayerBlue()->HasPlayedIllusion() == false && m_board->isBluePlayer()) {
        m_painter->drawButton(m_board->getPlayerBlue()->isPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);
    }

    if (m_board->getPlayerRed()->HasPlayedIllusion() == false && !m_board->isBluePlayer()) {
        m_painter->drawButton(m_board->getPlayerRed()->isPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);

    }
    //Draw the board, with the possible positions and played cards;
    for (const auto& possiblePosition : m_board->GetPossiblePositions()) {
        SDL_Rect renderRect{};
        renderRect.x = m_board->getCenterX() - (possiblePosition.GetX() * textureWidth);
        renderRect.y = m_board->getCenterY() - (possiblePosition.GetY() * textureHeight);
        renderRect.w = textureWidth;
        renderRect.h = textureHeight;

        if (m_painter->isMouseInRect(renderRect)) {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);

            if (m_board->isBluePlayer() && m_board->getPlayerBlue()->isGrabbingCard() && !m_painter->isPressingLeftClick()) {
                PlayingCard pushCard = *m_board->getPlayerBlue()->GetGrabbedCard();
                pushCard.SetBoardPosition(possiblePosition);
                pushCard.SetCoordinates({ renderRect.x, renderRect.y });
                if (m_board->getPlayerBlue()->HasPlayedIllusion() == false && m_board->getPlayerBlue()->isPlayingIllusion())
                    pushCard.SetIllussion(true);

                CardStatus status = m_board->pushNewCard(pushCard);

                if (status == ON_BOARD) {
                    m_board->getPlayerBlue()->removeCard(*m_board->getPlayerBlue()->GetGrabbedCard());
                    m_board->setIsBluePlayer(false);
                    if (pushCard.isIllusion())
                    {
                        for (auto& card : m_board->GetPlayedPositions())
                            if (card.second.back() == pushCard)
                                card.second.back().SetIllussion(m_board->getPlayerBlue()->isPlayingIllusion());
                        m_board->getPlayerBlue()->SetHasPlayedIllusion();
                    }
                    m_board->CheckStatus(m_currentState);
                }
                else if (status == IN_HAND) {
                    m_board->returnCardToDeck(*m_board->getPlayerBlue()->GetGrabbedCard());
                }
                else if (status == REMOVED) {
                    m_board->getPlayerBlue()->removeCard(*m_board->getPlayerBlue()->GetGrabbedCard());
                }

                //returning cards to deck if they are moved
                for (PlayingCard& card : m_board->getPlayerBlue()->GetCards()) {
                    m_board->returnCardToDeck(card);
                }
            }
            else if (m_board->getPlayerRed()->isGrabbingCard() && !m_painter->isPressingLeftClick()) {
                PlayingCard pushCard = *m_board->getPlayerRed()->GetGrabbedCard();
                pushCard.SetBoardPosition(possiblePosition);
                pushCard.SetCoordinates({ renderRect.x, renderRect.y });
                if (m_board->getPlayerRed()->HasPlayedIllusion() == false && m_board->getPlayerRed()->isPlayingIllusion())
                    pushCard.SetIllussion(true);

                CardStatus status = m_board->pushNewCard(pushCard);

                if (status == ON_BOARD) {
                    m_board->getPlayerRed()->removeCard(*m_board->getPlayerRed()->GetGrabbedCard());
                    if (pushCard.isIllusion()) {
                        for (auto& card : m_board->GetPlayedPositions())
                            if (card.second.back() == pushCard)
                                card.second.back().SetIllussion(m_board->getPlayerRed()->isPlayingIllusion());
                        m_board->getPlayerRed()->SetHasPlayedIllusion();
                    }
                    m_board->setIsBluePlayer(true);
                    m_board->CheckStatus(m_currentState);
                }
                else if (status == IN_HAND) {
                    m_board->returnCardToDeck(*m_board->getPlayerRed()->GetGrabbedCard());
                }
                else if (status == REMOVED) {
                    m_board->getPlayerRed()->removeCard(*m_board->getPlayerRed()->GetGrabbedCard());
                }

                //returning cards to deck if they are moved
                for (PlayingCard& card : m_board->getPlayerRed()->GetCards()) {
                    m_board->returnCardToDeck(card);
                }
            }
        }
        else {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 255, 255);
        }
        SDL_RenderDrawRect(m_painter->GetRenderer(), &renderRect);

    }

    for (const auto& cards = m_board->GetPlayedCards(); const auto & card : cards) {
        if (card.isIllusion())
        {
            if (card.GetColor() == BLUE)
                m_painter->drawCard(card, m_board->getBlueIllusionTexture()->getTexture());
            else if (card.GetColor() == RED)
            {
                m_painter->drawCard(card, m_board->getRedIllusionTexture()->getTexture());
            }
        }
        else
        {
            m_painter->drawCard(card, card.GetTexture()->getTexture());
        }
    }

    //Iterate each players' cards and draw them onto the screen
    //This is where all the in game logic will go
    for (auto& card : m_board->getPlayerBlue()->GetCards()) {
        SDL_Rect cardRect{};
        cardRect.x = card.GetCoordinates().GetX();
        cardRect.y = card.GetCoordinates().GetY();
        cardRect.w = textureWidth;
        cardRect.h = textureHeight;
        if (m_board->isBluePlayer()) {
            m_painter->drawCard(card, card.GetTexture()->getTexture());
            if (m_painter->isMouseInRect(cardRect)) {
                SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);
                SDL_RenderDrawRect(m_painter->GetRenderer(), &cardRect);
                if (m_painter->isPressingLeftClick() && !m_board->getPlayerBlue()->isGrabbingCard()) {
                    m_board->getPlayerBlue()->SetIsGrabbingCard(true);
                    m_board->getPlayerBlue()->SetGrabbedCard(&card);
                }
                if (m_board->getPlayerBlue()->isGrabbingCard()) {
                    if (m_board->getPlayerBlue()->GetGrabbedCard()->GetId() == card.GetId()) {
                        //std::cout << "Player blue is grabbing a card\n";
                        Coordinates mousePos = m_painter->getMousePos();
                        mousePos.SetX(mousePos.GetX() - (textureWidth / 2));
                        mousePos.SetY(mousePos.GetY() - (textureHeight / 2));
                        card.SetCoordinates(mousePos);
                    }
                }
            }
        }
        else {
            m_painter->drawCard(card, m_board->getBlueIllusionTexture()->getTexture());
        }
    }
    for (auto& card : m_board->getPlayerRed()->GetCards()) {
        SDL_Rect cardRect{};
        cardRect.x = card.GetCoordinates().GetX();
        cardRect.y = card.GetCoordinates().GetY();
        cardRect.w = textureWidth;
        cardRect.h = textureHeight;
        if (!m_board->isBluePlayer()) {
            m_painter->drawCard(card, card.GetTexture()->getTexture());
            if (m_painter->isMouseInRect(cardRect)) {
                SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);
                SDL_RenderDrawRect(m_painter->GetRenderer(), &cardRect);

                if (m_painter->isPressingLeftClick() && !m_board->getPlayerRed()->isGrabbingCard()) {
                    m_board->getPlayerRed()->SetIsGrabbingCard(true);
                    m_board->getPlayerRed()->SetGrabbedCard(&card);
                }
                if (m_board->getPlayerRed()->isGrabbingCard()) {
                    if (m_board->getPlayerRed()->GetGrabbedCard()->GetId() == card.GetId()) {
                        //std::cout << "Player red is grabbing a card\n";
                        Coordinates mousePos = m_painter->getMousePos();
                        mousePos.SetX(mousePos.GetX() - (textureWidth / 2));
                        mousePos.SetY(mousePos.GetY() - (textureHeight / 2));
                        card.SetCoordinates(mousePos);
                    }
                }
            }
        }
        else {
            m_painter->drawCard(card, m_board->getRedIllusionTexture()->getTexture());
        }
    }
    if (!m_painter->isPressingLeftClick() && (m_board->getPlayerRed()->isGrabbingCard() || m_board->getPlayerBlue()->isGrabbingCard())) {
        //std::cout << "Player stopped grabbing a card\n";
        m_board->getPlayerRed()->SetIsGrabbingCard(false);
        m_board->getPlayerBlue()->SetIsGrabbingCard(false);
    }
}

