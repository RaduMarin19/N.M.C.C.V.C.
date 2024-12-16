#include "Game.h"
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
            
            m_painter->SetEvent(e);
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
                if (m_painter->DrawLoginPage()) {
                    m_currentState = MODE_SELECTION;
                    drawThisFrame = true;
                }
            }

            if (m_currentState == MODE_SELECTION && !drawThisFrame) {

                //Draw the mode selection menu
                m_painter->DrawModeSelection();

                //Check which button the player has pressed, and initialize the cards accordingly
                if (m_painter->IsTrainingActive()) {
                    m_currentState = TRAINING_MODE;
                    m_board->GeneratePlayerCards(GameMode::Training);
                    drawThisFrame = true;
                }
                else if (m_painter->IsElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    m_board->SetGameMode(GameMode::Elemental);
                    m_board->GeneratePlayerCards(GameMode::Elemental);
                    drawThisFrame = true;
                }
                else if (m_painter->IsMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    m_board->SetGameMode(GameMode::MageDuel);
                    m_board->GeneratePlayerCards(GameMode::MageDuel);
                    drawThisFrame = true;
                }
                else if (m_painter->IsTournamentActive())
                {
                    m_currentState = TOURNAMENT;
					drawThisFrame = true;
                }
                else if (m_painter->IsQuickMatchActive())
                {
                    m_currentState = QUICK_MODE;
					m_board->SetGameMode(GameMode::QuickMode);
                    m_board->GeneratePlayerCards(GameMode::QuickMode);
                    drawThisFrame = true;
                }
            }

            if (m_currentState == TOURNAMENT && !drawThisFrame)
            {
                m_painter->DrawTournamentModeSelection();
                if (m_painter->IsElementalActive())
                {
                    m_currentState = ELEMENTAL_BATTLE;
                    m_board->SetGameMode(GameMode::Elemental);
                    m_board->GeneratePlayerCards(GameMode::Elemental);
                    drawThisFrame = true;
                }
                else if (m_painter->IsMageDuelActive())
                {
                    m_currentState = MAGE_DUEL;
                    m_board->SetGameMode(GameMode::MageDuel);
                    m_board->GeneratePlayerCards(GameMode::MageDuel);
                    drawThisFrame = true;
                }
            }

            if (m_currentState == RED_PLAYER_WON || m_currentState == BLUE_PLAYER_WON || m_currentState == TIE) {
                std::string message = (m_currentState == RED_PLAYER_WON) ? "RED player WON!" :
                    (m_currentState == BLUE_PLAYER_WON) ? "Blue player WON!" : "TIE!";
                m_painter->DrawText(message, { SCREEN_WIDTH / 2, 50 }, 14, true);

                bool isPressed = false;
                m_painter->DrawButton(isPressed, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Return to menu!", 14);
                if (isPressed) {
                    m_currentState = MODE_SELECTION;
                    drawThisFrame = true;
                    m_painter->ResetGameModes();
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
    if (player.HasPlayedIllusion() == false && player.IsPlayingIllusion()) {
        pushCard->SetIllusion(true);
        player.SetHasPlayedIllusion();
    }

    CardStatus status = m_board->PushNewCard(*pushCard);

    if (status == ON_BOARD) {
        player.RemoveCardFromHand(*pushCard);
        if (pushCard->IsIllusion())
        {
            for (auto& card : m_board->GetPlayedPositions())
                if (card.second.back() == *pushCard)
                    card.second.back().SetIllusion(player.IsPlayingIllusion());
            player.SetHasPlayedIllusion();
        }
        m_board->CheckStatus(m_currentState);
        if (m_board->CanUseExplosion()) {
            m_explosionTurn = true;
        }
        else {
            m_board ->ChangeTurn(); 
        }
        player.SetIsPlayingAshes(false);
    }
    else if (status == IN_HAND) {
        m_board->ReturnCardToDeck(*pushCard);
    }
    else if (status == REMOVED) {
        player.AddRemovedCard(player.GetCards(), *pushCard);
        player.SetIsPlayingAshes(false);
    }
}

void Game::PlaySpellCard(Player& player,SpellCard* spellCard, SDL_Rect& renderRect, const Coordinates& possiblePosition) {
    spellCard->SetCoordinates({ renderRect.x, renderRect.y });

    ElementalType spell = spellCard->GetSpell(); //getting the spell type

    switch (spell) {
        case ElementalType::FIRE: 
        {
            try {
                Color playerColor = player.GetCards().back().GetColor();
                if (m_board->GetCardColorAtPosition(possiblePosition) != playerColor) //checking if the color of the chosen card is different than that of the player
                {
                    if (m_board->RemoveIllusion(possiblePosition))   //if the illusion is successfully removed then remove the spell card
                        m_board->RemoveSpell(spellCard);
                }
                else {
                    m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                }
            }
            catch (const std::runtime_error& error) {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }

        case ElementalType::SQUALL:
            m_board->ReturnCoveredCards();      //returns all covered cards to the players hand
            m_board->RemoveSpell(spellCard);    //then remove the spell card
            m_board->ChangeTurn();              //player loses its turn
            break;

        case ElementalType::ASHES: {
            std::vector<PlayingCard>& cards = player.GetRemovedCards();
            if (cards.size() > 0) {  //if the player has at least one removed cards(avoiding errors)
                player.SetIsPlayingAshes(true);         //this will determine from which deck the player will take cards
                m_board->RemoveSpell(spellCard);        //then remove the spell card
                for (decltype(auto) card : cards) {
                    m_board->ReturnCardToDeck(card);
                }
            }
            break;
        }

        case ElementalType::STORM: {
            try {
                short size = m_board->GetCardsAtPosition(possiblePosition).size();   //getting the size for nicer code

                if (size > 1) {                                                      //if the position has >= 2 cards
                   /* ExplosionCard* explosion = new ExplosionCard(m_board->GetTableSize());
                    std::vector <std::pair<Coordinates,ExplosionType>> ex;
                    ex.push_back({ possiblePosition,ExplosionType::TOTAL_DELETE });
                    explosion->makeExplosionFromVector(ex);*/
                    if (true/*m_board->validateBoardAfterEffect(explosion)*/) {
                        while (size) {                                                   //delete all cards from the game
                            m_board->DeleteCardAtPosition(possiblePosition);
                            --size;
                        }
                        m_board->RemoveSpell(spellCard); //then remove the spell card
                    }
                }
                else {
                    m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                }
            } 
            catch (const std::runtime_error& error) {   //will throw if there is no cards at the played position
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }
                                 
        case ElementalType::BOULDER:
            try {
                auto& cards= m_board->GetCardsAtPosition(possiblePosition);
                if (cards.back().IsIllusion()) {
                    m_board->SetCanCoverIllusion(true);     //telling the board that the player will be able to cover a illusion
                    m_board->RemoveSpell(spellCard); //then remove the spell card
                }
            }
            catch (const std::runtime_error& error) {   //will throw if there is no cards at the played position
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
           break;
        case ElementalType::SUPPORT:
        {
            try {
                Color playerColor = player.GetCards().back().GetColor();
                if (m_board->GetCardColorAtPosition(possiblePosition) == playerColor) //checking if the color of the chosen card is the same as the color of the player
                {
                    if (m_board->ChangeCardValue(m_board->GetCardsAtPosition(possiblePosition).back(), 1)) { //if the change of value was sucessful
                        m_board->RemoveSpell(spellCard); //then remove the spell card
                        m_board->ChangeTurn();
                    }
                    else
                        m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                }
            }
            catch (const std::runtime_error& error) {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }
        case ElementalType::SHATTER:
        {
            try {
                Color playerColor = player.GetCards().back().GetColor();
                if (m_board->GetCardColorAtPosition(possiblePosition) != playerColor) //checking if the color of the chosen card is different than that of the player
                {
                    if (m_board->ChangeCardValue(m_board->GetCardsAtPosition(possiblePosition).back(), -1)) { //if the change of value was sucessful
                        m_board->RemoveSpell(spellCard); //then remove the spell card
                        m_board->ChangeTurn();
                    }
                    else
                        m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                }
            }
            catch (const std::runtime_error& error) {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }
        case ElementalType::LAVA:
        {
            try {
                short cardValue = m_board->GetCardsAtPosition(possiblePosition).back().GetValue();  //getting the value of the card under the spell
                short cardsCount = 0;
                for (const PlayingCard& card : m_board->GetPlayedCards()) {
                    if (card.GetValue() == cardValue) {
                        ++cardsCount;                                       //counting the cards with equal value
                    }
                }
                if (cardsCount >= 2) {
                    for (const auto& card : m_board->GetPlayedCards()) {                    
                        if (card.GetValue() == cardValue) {     
                            m_board->ReturnTopCardAtPosition(card.GetBoardPosition());        //returning the top cards that are found at each position with equal value
                        }
                    }
                    m_board->RemoveSpell(spellCard); //then remove the spell card
                    m_board->ChangeTurn();
                }
                else {
                    m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                }
            }
            catch (const std::runtime_error& error) {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }
        case ElementalType::WHIRLWIND:
            try {
                if (m_board->GetCardsAtPosition(possiblePosition).back().GetColor() != player.GetColor()) {
                    m_board->ReturnTopCardAtPosition(possiblePosition);
                    m_board->RemoveSpell(spellCard); //then remove the spell card
                    m_board->ChangeTurn();
                }
                else {
                    m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                }
            }
            catch (const std::runtime_error& error) {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        case ElementalType::MIST:
        {
            Color playerColor = player.GetColor();
            for (auto& [coordinates, cards] : m_board->GetPlayedPositions()) {
                for (auto& card : cards) {
                    if (card.IsIllusion() && card.GetColor() == playerColor) {  //cannot play 2 illusions at the same time
                        m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                        return;
                    }
                }
            }
            if (player.HasPlayedIllusion() == true)         //can only play mist illusion after playing the regular one
                player.IsPlayingIllusion() = true;
            m_board->RemoveSpell(spellCard); //then remove the spell card
        }
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
        m_painter->DrawButton(m_board->GetPlayerBlue()->IsPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);
    }

    if (m_board->GetPlayerRed()->HasPlayedIllusion() == false && !m_board->IsBluePlayer()) {
        m_painter->DrawButton(m_board->GetPlayerRed()->IsPlayingIllusion(), { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 100 }, 120, 50, "Play illusion!", 14);
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
            m_board->ChangeTurn();
        }
    }

    if (!m_painter->IsPressingLeftClick() && (m_board->GetPlayerRed()->IsGrabbingCard() || m_board->GetPlayerBlue()->IsGrabbingCard())) {
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

        if (m_painter->IsMouseInRect(renderRect)) {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);

            if (m_board->IsBluePlayer() && m_board->GetPlayerBlue()->IsGrabbingCard() && !m_painter->IsPressingLeftClick()) {
                PlayerTurn(*m_board->GetPlayerBlue(), renderRect, possiblePosition);
            }
            else if (m_board->GetPlayerRed()->IsGrabbingCard() && !m_painter->IsPressingLeftClick()) {
                PlayerTurn(*m_board->GetPlayerRed(), renderRect, possiblePosition);
            }
        }
        else {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 255, 255);
        }
        SDL_RenderDrawRect(m_painter->GetRenderer(), &renderRect);
    }

    for (const auto& cards = m_board->GetPlayedCards(); const auto & card : cards) {
        if (card.IsIllusion())
        {
            if (card.GetColor() == BLUE)
                m_painter->DrawCard(card, m_board->GetPlayerBlue()->GetIllusionTexture().GetTexture());
            else if (card.GetColor() == RED)
            {
                m_painter->DrawCard(card, m_board->GetPlayerRed()->GetIllusionTexture().GetTexture());
            }
        }
        else
        {
            m_painter->DrawCard(card, card.GetTexture()->GetTexture());
        }
    }
}

void Game::HandleCardMovement(Player* player,Card& card) {
    SDL_Rect cardRect{};
    cardRect.x = card.GetCoordinates().GetX();
    cardRect.y = card.GetCoordinates().GetY();
    cardRect.w = textureWidth;
    cardRect.h = textureHeight;
    if (m_painter->IsMouseInRect(cardRect)) {
        SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);
        SDL_RenderDrawRect(m_painter->GetRenderer(), &cardRect);
        if (m_painter->IsPressingLeftClick() && !player->IsGrabbingCard()) {
            player->SetIsGrabbingCard(true);
            player->SetGrabbedCard(&card);
        }
        if (player->IsGrabbingCard()) {
            if (player->GetGrabbedCard()->GetId() == card.GetId()) {
                //std::cout << "Player blue is grabbing a card\n";
                Coordinates mousePos = m_painter->GetMousePos();
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
            m_painter->DrawCard(card, card.GetTexture()->GetTexture());
            if (!m_explosionTurn) {
                HandleCardMovement(player, card);
            }
        }
        else {
            m_painter->DrawCard(card, player->GetIllusionTexture().GetTexture());
        }
    };

    std::vector<PlayingCard>& cards = (isPlayersTurn && player->IsPlayingAshes()) ? player->GetRemovedCards() : player->GetCards();

    for (auto& card : cards) {
        
        DrawAndHandleCard(player, card);
    }
    if (isPlayersTurn) {
        decltype(auto) spells = m_board->GetSpells();

        if (spells) {
            auto& [spellCard1, spellCard2] = *spells;
            if(spellCard1 != nullptr)
                DrawAndHandleCard(player, *spellCard1);
            if (spellCard2 != nullptr)
                DrawAndHandleCard(player, *spellCard2);
        }
    }
}

bool Game::ExplosionTurn(){
    static bool checkExplosion = false;
    static bool isExplosionValidated = false;
    bool dontExplode = false;
    m_painter->DrawButton(dontExplode, {SCREEN_WIDTH - 700, SCREEN_HEIGHT - 100 }, 150, 50, "Skip explosion!", 14);

    if (dontExplode) {
        return true;
    }

    if(!isExplosionValidated) {
        m_board->SetValidatedExplosion(m_board->GetExplosion());
        isExplosionValidated = true;
    }

    static bool rotate = false;
    m_painter->DrawButton(rotate, { SCREEN_WIDTH / 2 - textureWidth * 3, SCREEN_HEIGHT - 500 }, 128, 128, "Rotate!", 14);

    {
        SDL_Rect explosionRect{ SCREEN_WIDTH / 2 - textureWidth * 3, SCREEN_HEIGHT - 500, 128, 128 };
        m_painter->DrawTexturedRect(explosionRect, m_board->GetExplosionBoardTexture()->GetTexture());
        auto explosion = m_board->GetExplosion();
        decltype(auto) explosionMask = explosion->GetExplosionMask();
        for (int i = 0; i < m_board->GetTableSize(); i++) {
            for (int j = 0; j < m_board->GetTableSize(); j++) {
                SDL_Rect spriteRect{ explosionRect.x + 12 + (i * 34), explosionRect.y + 6 + (j * 32), 32, 32 };
                if (explosionMask[i][j] == ExplosionType::HOLE) {
                    m_painter->DrawTexturedRect(spriteRect, m_board->GetExplosionSprite(2)->GetTexture());
                }
                else if (explosionMask[i][j] == ExplosionType::DELETE) {
                    m_painter->DrawTexturedRect(spriteRect, m_board->GetExplosionSprite(0)->GetTexture());
                }
                else if (explosionMask[i][j] == ExplosionType::RETURN) {
                    m_painter->DrawTexturedRect(spriteRect, m_board->GetExplosionSprite(1)->GetTexture());
                }
            }
        }

        m_drawThisFrame = false;
        bool exploded = false;
        m_painter->DrawButton(exploded, { SCREEN_WIDTH - 850, SCREEN_HEIGHT - 100 }, 100, 50, "Explode!", 14);

        if (exploded) {
            m_board->Explode();
            return true;
        }
        if (rotate) {
            m_board->GetExplosion()->RotateExplosion();
            rotate = false;
            isExplosionValidated = false;
        }
    }
    return false;
}