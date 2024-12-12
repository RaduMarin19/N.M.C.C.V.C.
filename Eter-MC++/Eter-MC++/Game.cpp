﻿#include "Game.h"
#include <thread>

Game::Game()
    : m_currentState(WELCOME_SCREEN) {
    m_painter = std::make_unique<Graphics>();
    m_board = std::make_unique<GameBoard>(m_painter.get()->GetRenderer());
}

Game& Game::GetInstance()
{
    static Game gameInstance;
    return gameInstance;
}

void Game::SetGameState(GameState state)
{
    m_currentState = state;
}

GameState Game::GetGameState() const
{
    return m_currentState;
}

void Game::Run() {

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
                    m_board->GeneratePlayerCards(GameMode::Training);
                    m_board->SetIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (m_painter->isElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    m_board->SetGameMode(GameMode::Elemental);
                    m_board->GeneratePlayerCards(GameMode::Elemental);
					m_board->SetIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (m_painter->isMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    m_board->SetGameMode(GameMode::MageDuel);
                    m_board->GeneratePlayerCards(GameMode::MageDuel);
                    m_board->SetIsBluePlayer(true);
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
					m_board->SetGameMode(GameMode::QuickMode);
                    m_board->GeneratePlayerCards(GameMode::QuickMode);
                    m_board->SetIsBluePlayer(true);
                    drawThisFrame = true;
                }
            }

            if (m_currentState == TOURNAMENT && !drawThisFrame)
            {
                m_painter->drawTournamentModeSelection();
                if (m_painter->isElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    m_board->SetGameMode(GameMode::Elemental);
                    m_board->GeneratePlayerCards(GameMode::Elemental);
					m_board->SetIsBluePlayer(true);
                    drawThisFrame = true;
                }
                else if (m_painter->isMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    m_board->SetGameMode(GameMode::MageDuel);
                    m_board->GeneratePlayerCards(GameMode::MageDuel);
                    m_board->SetIsBluePlayer(true);
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
                    m_board->Clear();
                    m_explosionTurn = false;
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

void Game::PlayRegularCard(Player& player,PlayingCard* pushCard, SDL_Rect& renderRect, const Coordinates& possiblePosition) {
    pushCard->SetBoardPosition(possiblePosition);
    pushCard->SetCoordinates({ renderRect.x, renderRect.y });
    if (player.HasPlayedIllusion() == false && player.isPlayingIllusion()) {
        pushCard->SetIllussion(true);
        player.SetHasPlayedIllusion();
    }

    CardStatus status = m_board->PushNewCard(*pushCard);

    if (status == ON_BOARD) {
        player.removeCard(*pushCard);
        if (pushCard->isIllusion())
        {
            for (auto& card : m_board->GetPlayedPositions())
                if (card.second.back() == *pushCard)
                    card.second.back().SetIllussion(player.isPlayingIllusion());
            player.SetHasPlayedIllusion();
        }
        m_board->CheckStatus(m_currentState);
        if (m_board->CanUseExplosion()) {
            m_explosionTurn = true;
        }
        else {
            m_board->SetIsBluePlayer(!m_board->IsBluePlayer());
        }
    }
    else if (status == IN_HAND) {
        m_board->ReturnCardToDeck(*pushCard);
    }
    else if (status == REMOVED) {
        player.removeCard(*pushCard);
    }
}

void Game::PlaySpellCard(Player& player,SpellCard* spellCard, SDL_Rect& renderRect, const Coordinates& possiblePosition) {
    spellCard->SetCoordinates({ renderRect.x, renderRect.y });

    ElementalType spell = spellCard->GetSpell();

    switch (spell) {
        case ElementalType::FIRE: {
            Color playerColor = player.GetCards().back().GetColor();
            if (m_board->GetCardColorAtPosition(possiblePosition) != playerColor)
            {
                if (m_board->RemoveIllusion(possiblePosition))
                  m_board->RemoveSpell(spellCard);
            }
            break;
        }
        case ElementalType::SQUALL:
            m_board->ReturnCoveredCards();
            m_board->RemoveSpell(spellCard);
            break;
    }
}

void Game::PlayerTurn(Player& player, SDL_Rect& renderRect, const Coordinates& possiblePosition) {
    Card* pushCard = player.GetGrabbedCard();

    if (pushCard) {
        if (PlayingCard* playingCard = dynamic_cast<PlayingCard*>(pushCard)) {
            PlayRegularCard(player, playingCard, renderRect, possiblePosition);
        }
        else if (SpellCard* spellCard = dynamic_cast<SpellCard*>(pushCard)) {
            if (spellCard) 
                PlaySpellCard(player, spellCard,renderRect,possiblePosition);
        }
    }
    
    //returning cards to deck if they are moved
    for (PlayingCard& card : player.GetCards()) {
        m_board->ReturnCardToDeck(card);
    }
}

void Game::HandleBoardState() {

    if (m_currentState == TOURNAMENT)
        return;

    if (m_currentState == TRAINING_MODE || m_currentState == QUICK_MODE)
        m_board->SetTable(3);
    else
        m_board->SetTable(4);

    m_board->UpdateBoardCenter();

    // Common logic for all modes

    //drawing the play illusion buttons
    if (m_board->GetPlayerBlue()->HasPlayedIllusion() == false && m_board->IsBluePlayer()) {
        m_painter->drawButton(m_board->GetPlayerBlue()->isPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);
    }

    if (m_board->GetPlayerRed()->HasPlayedIllusion() == false && !m_board->IsBluePlayer()) {
        m_painter->drawButton(m_board->GetPlayerRed()->isPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);
    }

    //Draw the board, with the possible positions and played cards;
    DrawBoard();

    //Iterate each players' cards and draw them onto the screen
    //This is where all the in game logic will go
    DrawPlayersCards(m_board->GetPlayerBlue(), m_board->IsBluePlayer());
    DrawPlayersCards(m_board->GetPlayerRed(), !m_board->IsBluePlayer());



    if (m_explosionTurn){
        if (ExplosionTurn() == true) {
            m_explosionTurn = false;
            m_board->SetIsBluePlayer(!m_board->IsBluePlayer());
        }
    }

    if (!m_painter->isPressingLeftClick() && (m_board->GetPlayerRed()->isGrabbingCard() || m_board->GetPlayerBlue()->isGrabbingCard())) {
        //std::cout << "Player stopped grabbing a card\n";
        m_board->GetPlayerRed()->SetIsGrabbingCard(false);
        m_board->GetPlayerBlue()->SetIsGrabbingCard(false);
    }
}

void Game::DrawBoard() {
    for (const auto& possiblePosition : m_board->GetPossiblePositions()) {
        SDL_Rect renderRect{};
        renderRect.x = m_board->GetCenterX() - (possiblePosition.GetX() * textureWidth);
        renderRect.y = m_board->GetCenterY() - (possiblePosition.GetY() * textureHeight);
        renderRect.w = textureWidth;
        renderRect.h = textureHeight;

        if (m_painter->isMouseInRect(renderRect)) {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);

            if (m_board->IsBluePlayer() && m_board->GetPlayerBlue()->isGrabbingCard() && !m_painter->isPressingLeftClick()) {
                PlayerTurn(*m_board->GetPlayerBlue(), renderRect, possiblePosition);
            }
            else if (m_board->GetPlayerRed()->isGrabbingCard() && !m_painter->isPressingLeftClick()) {
                PlayerTurn(*m_board->GetPlayerRed(), renderRect, possiblePosition);
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
                m_painter->drawCard(card, m_board->GetPlayerBlue()->GetIllusionTexture().GetTexture());
            else if (card.GetColor() == RED)
            {
                m_painter->drawCard(card, m_board->GetPlayerRed()->GetIllusionTexture().GetTexture());
            }
        }
        else
        {
            m_painter->drawCard(card, card.GetTexture()->GetTexture());
        }
    }
}

void Game::HandleCardMovement(Player* player,Card& card) {
    SDL_Rect cardRect{};
    cardRect.x = card.GetCoordinates().GetX();
    cardRect.y = card.GetCoordinates().GetY();
    cardRect.w = textureWidth;
    cardRect.h = textureHeight;
    if (m_painter->isMouseInRect(cardRect)) {
        SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);
        SDL_RenderDrawRect(m_painter->GetRenderer(), &cardRect);
        if (m_painter->isPressingLeftClick() && !player->isGrabbingCard()) {
            player->SetIsGrabbingCard(true);
            player->SetGrabbedCard(&card);
        }
        if (player->isGrabbingCard()) {
            if (player->GetGrabbedCard()->GetId() == card.GetId()) {
                //std::cout << "Player blue is grabbing a card\n";
                Coordinates mousePos = m_painter->getMousePos();
                mousePos.SetX(mousePos.GetX() - (textureWidth / 2));
                mousePos.SetY(mousePos.GetY() - (textureHeight / 2));
                card.SetCoordinates(mousePos);
            }
        }
    }
}

void Game::DrawPlayersCards(Player* player,bool isPlayersTurn) {
    auto DrawAndHandleCard = [&](Player* player, Card& card) {
        if (isPlayersTurn) {
            m_painter->drawCard(card, card.GetTexture()->GetTexture());
            if (!m_explosionTurn) {
                HandleCardMovement(player, card);
            }
        }
        else {
            m_painter->drawCard(card, player->GetIllusionTexture().GetTexture());
        }
    };

    for (auto& card : player->GetCards()) {
        
        DrawAndHandleCard(player, card);
    }
    if (isPlayersTurn) {
        decltype(auto) spells = m_board->GetSpells();

        if (spells) {
            auto& [spellCard1, spellCard2] = *spells;
            if(spellCard1!=nullptr)
            DrawAndHandleCard(player, *spellCard1);
            if (spellCard2!=nullptr)
            DrawAndHandleCard(player, *spellCard2);
        }
    }
}

bool Game::ExplosionTurn(){
    static bool checkExplosion = false;
    bool dontExplode = false;
    m_painter->drawButton(dontExplode, { SCREEN_WIDTH - 1000, SCREEN_HEIGHT - 260 }, 120, 50, "Don`t Explode!", 14);

    if (dontExplode) {
        checkExplosion = false;
        return true;
    }

    if (!checkExplosion) {
        m_painter->drawButton(checkExplosion, { SCREEN_WIDTH - 1000, SCREEN_HEIGHT - 100 }, 120, 50, "Check explosion!", 14);
    }
    if (checkExplosion) {
        {
            SDL_Rect explosionRect{ SCREEN_WIDTH / 2 - textureWidth * 3, SCREEN_HEIGHT - 500, 128, 128 };
            m_painter->drawTexturedRect(explosionRect, m_board->GetExplosionBoardTexture()->GetTexture());
            auto explosion = m_board->GetExplosion();
            decltype(auto) explosionMask = explosion->GetExplosionMask();
            for (int i = 0; i < m_board->GetTableSize(); i++) {
                for (int j = 0; j < m_board->GetTableSize(); j++) {
                    SDL_Rect spriteRect{ explosionRect.x + 12 + (i * 34), explosionRect.y + 6 + (j * 32), 32, 32 };
                    if (explosionMask[i][j] == ExplosionType::HOLE) {
                        m_painter->drawTexturedRect(spriteRect, m_board->GetExplosionSprite(2)->GetTexture());
                    }
                    else if (explosionMask[i][j] == ExplosionType::DELETE) {
                        m_painter->drawTexturedRect(spriteRect, m_board->GetExplosionSprite(0)->GetTexture());
                    }
                    else if (explosionMask[i][j] == ExplosionType::RETURN) {
                        m_painter->drawTexturedRect(spriteRect, m_board->GetExplosionSprite(1)->GetTexture());
                    }
                }

            }
        }
        m_drawThisFrame = false;
        bool exploded = false;
        m_painter->drawButton(exploded, { SCREEN_WIDTH - 750, SCREEN_HEIGHT - 100 }, 100, 50, "Explode!", 14);
        static bool rotate = false;
        m_painter->drawButton(rotate, { SCREEN_WIDTH - 550, SCREEN_HEIGHT - 100 }, 100, 50, "Rotate!", 14);
        
        if (exploded) {
            //if (board.ValidateExplosion())
            m_board->Explode();
            //else
            //   std::cout << "Explosion invalidates map!\n";
            return true;
        }
        if (rotate) {
            m_board->GetExplosion()->RotateExplosion();
            std::cout << "----------------\n";
            m_board->PrintExplosionMask();
            rotate = false;
        }
    }
    return false;
}