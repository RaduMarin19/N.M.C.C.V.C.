#include "Game.h"
#include <thread>

Game::Game()
    : m_currentState(GameState::WELCOME_SCREEN) {
    m_painter = std::make_unique<Graphics>();
    m_board = std::make_unique<GameBoard>(m_painter.get()->GetRenderer());
    m_selectedStack = nullptr;
    m_explosionTurn = false;
    m_quickModeTimer = 0;
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

void Game::Run()
{
    GetInstance().IRun();
}

void Game::IRun() {

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

            //if (drawThisFrame) continue;

            switch (m_currentState) {
                case GameState::WELCOME_SCREEN:{
                    if (m_painter->DrawLoginPage()) {
                        m_currentState = GameState::MODE_SELECTION;
                    }
                    break;
                }
                case GameState::MODE_SELECTION: {
                    HandleModeSelection();
                    break;
                }
                case GameState::TOURNAMENT: {
                     HandleTournamentSelection();
                     break;
                }
                case GameState::QUICK_MODE: {
                     HandleQuickModeSelection();
                     break;
                }
            }

            if ((m_currentState == GameState::RED_PLAYER_WON || m_currentState == GameState::BLUE_PLAYER_WON || m_currentState == GameState::TIE)) {
                HandleWin();
            }

            if (m_currentState == GameState::TRAINING_MODE || m_currentState == GameState::ELEMENTAL_BATTLE ||
                m_currentState == GameState::MAGE_DUEL || m_currentState == GameState::TOURNAMENT || 
                m_currentState == GameState::MAGE_ELEMENTAL) {
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

void Game::HandleTournamentSelection() {
    m_painter->DrawTournamentModeSelection(m_currentState);
    m_nextRoundState = m_currentState;

    if (m_currentState != GameState::TOURNAMENT) {
        if (m_currentState == GameState::TRAINING_MODE)
            m_board->SetTable(3);
        else
            m_board->SetTable(4);

        m_board->GeneratePlayerCards(m_currentState);
        m_board->InitializeExplosion();
    }
}

void Game::HandleQuickModeSelection() { //// QUICK MODE
    m_painter->DrawQuickModeSelection(m_currentState, m_quickModeTimer);

    if (m_currentState != GameState::TOURNAMENT) {
        if (m_currentState == GameState::TRAINING_MODE) {
            m_board->SetTable(3);
            m_board->setPlayingQuickMatch(true);
        }
        else {
            m_board->SetTable(4);
            m_board->setPlayingQuickMatch(true);
        }

        m_board->GeneratePlayerCards(m_currentState);
        m_board->InitializeExplosion();
        m_board->GetPlayerBlue()->setDeltaTime(SDL_GetTicks());
        m_board->GetPlayerRed()->setDeltaTime(SDL_GetTicks());
        m_board->GetPlayerBlue()->setTimeRemaining(m_quickModeTimer);
        m_board->GetPlayerRed()->setTimeRemaining(m_quickModeTimer);
    }
}

void Game::HandleModeSelection() {
    m_painter->DrawModeSelection(m_currentState);
    m_nextRoundState = m_currentState;

    bool loadSave = false;
    m_painter->DrawButton(loadSave, { SCREEN_WIDTH / 2 - 75, 400 }, 150, 40, "Load Save", 14);
    if (loadSave) {
        LoadSave();
    }
    bool switchedState = false;

    switch (m_currentState) {
    case GameState::TRAINING_MODE:
        m_board->SetTable(3);
        m_bestOf = 3;
        switchedState = true;
        break;
    case GameState::MAGE_DUEL:
        m_board->SetTable(4);
        m_bestOf = 5;
        switchedState = true;
        break;
    case GameState::ELEMENTAL_BATTLE:
        m_board->SetTable(4);
        m_bestOf = 5;
        switchedState = true;
        break;
    }

    if (switchedState == true && !loadSave) {
        m_board->GeneratePlayerCards(m_currentState);
        m_board->InitializeExplosion();
    }
}

bool Game::HandleWin() {
    std::string message;
    static bool incrementWin = true;

    if (m_currentState == GameState::RED_PLAYER_WON || m_board->GetPlayerBlue()->GetTimeRemaining() <= 0)
    {
        message = "RED player WON";
        if(incrementWin)
        {
            m_board->IncreaseRoundsWon(GameState::RED_PLAYER_WON);
        }
        std::cout << "Red rounds won: " << m_board->GetRedRoundsWon() << '\n';
        incrementWin = false;
    }
    else if(m_currentState == GameState::BLUE_PLAYER_WON || m_board->GetPlayerRed()->GetTimeRemaining() <= 0)
    {
        message = "BLUE player WON";
        if(incrementWin)
        {
            m_board->IncreaseRoundsWon(GameState::BLUE_PLAYER_WON);
        }
        std::cout << "Blue rounds won: " << m_board->GetBlueRoundsWon() << '\n';
        incrementWin = false;
    }
    else
    {
        message = "TIE!";
    }

    bool isNextRoundButton = false;
    bool isReset = false;

    if(m_board->GetBlueRoundsWon() == m_bestOf / 2 + 1 || m_board->GetRedRoundsWon() == m_bestOf / 2 + 1 || m_board->getPlayingQuickMatch())
    {
        m_painter->DrawText(message + " the MATCH!", {SCREEN_WIDTH / 2, 50}, 14, true);
        m_painter->DrawButton(isReset, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Return to menu!", 14);
    }
    else
    {
        if(message == "TIE!")
            m_painter->DrawText(message, { SCREEN_WIDTH / 2, 50 }, 14, true);
        else
            m_painter->DrawText(message + " the ROUND!", { SCREEN_WIDTH / 2, 50 }, 14, true);
        m_painter->DrawButton(isNextRoundButton, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Play next round!", 14);
        /*m_painter->DrawButton(isReset, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Play next round!", 14);*/
    }

    if (isReset) {
        m_currentState = GameState::MODE_SELECTION;
        m_board->Clear();
        m_board->SetBlueRoundsWon(0);
        m_board->SetRedRoundsWon(0);
        m_explosionTurn = false;
        incrementWin = true;
        return true;
    }
    else if (isNextRoundButton) {
        m_currentState = m_nextRoundState;
        m_board->ResetRound(m_currentState);
        m_explosionTurn = false;
        incrementWin = true;
        return true;
    }
    return false;
}

void Game::PlayRegularCard(Player& player,PlayingCard* pushCard, SDL_Rect& renderRect, const Coordinates& possiblePosition) {

    pushCard->SetBoardPosition(possiblePosition);
    pushCard->SetCoordinates({ renderRect.x, renderRect.y });
    if (pushCard->IsEter()) {
        player.IsPlayingIllusion() = false;
    }

    if (player.HasPlayedIllusion() == false && player.IsPlayingIllusion()) { //if player can play a illusion
        pushCard->SetIllusion(true);
        player.SetHasPlayedIllusion();
    }

    if (m_board->IsPlayingMirage()) {
        pushCard->SetIllusion(true);
        m_board->SetCanCoverIllusion(true);
    }
    m_selectedStack = nullptr; //resetting the selected stack if a player doesnt select the second one too

    CardStatus status = m_board->PushNewCard(*pushCard);

    if (status == ON_BOARD) {
        m_board->SetBlockedRow(100);
        player.RemoveCardFromHand(*pushCard);
        auto playedCardDeque = m_board->GetCardsAtPosition(possiblePosition);
        player.AddCardToOrderStack(playedCardDeque[playedCardDeque.size() - 1]);
        if (pushCard->IsIllusion())
        {
            for (auto& cards : m_board->GetPlayedPositions())
                if (cards.second.back() == *pushCard)
                    cards.second.back().SetIllusion(player.IsPlayingIllusion());
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
        m_board->SetBlockedRow(100);
        player.AddRemovedCard(player.GetCards(), *pushCard);
        player.SetIsPlayingAshes(false);
    }
}
void Game::PlayWizardCard(Player& player, WizardCard* wizardCard, SDL_Rect& renderRect, const Coordinates& possiblePosition) {
    wizardCard->SetCoordinates({ renderRect.x, renderRect.y });

    try {
        auto& cards = m_board->GetCardsAtPosition(possiblePosition);
        if (cards.back().IsEter()) {
            m_board->ReturnCardToDeck(*wizardCard);
            return;
        }
    }
    catch (std::runtime_error& e) {
        //nothing
    }

    WizardType wizard = wizardCard->GetWizard();

    switch (wizard) {
    case WizardType::REMOVE_OPPONENT_CARD:
    {
        try {
            decltype(auto) cardDeck = m_board->GetCardsAtPosition(possiblePosition);

            if (cardDeck.size() >= 2 && cardDeck[cardDeck.size() - 2].GetColor() == player.GetColor() && cardDeck[cardDeck.size() - 1].GetColor() != player.GetColor()) {
                m_board->DeleteCardAtPosition(possiblePosition);
                m_board->ChangeTurn();              //player loses its turn
                player.RemoveWizard();
            }
            else {
                m_board->ReturnCardToDeck(*wizardCard);
            }
        }
        catch (const std::runtime_error& e) {
            m_board->ReturnCardToDeck(*wizardCard);
        }
        break;
    }
    case WizardType::REMOVE_ROW:
        if (m_board->RemoveRow(possiblePosition.GetY(),player.GetColor()))
        {
            m_board->ChangeTurn();              //player loses its turn
            player.RemoveWizard();
        }
        else {
            m_board->ReturnCardToDeck(*wizardCard);
        }

        break;
    case WizardType::COVER_OPPONENT_CARD:
        m_board->SetIsPlayingCoverOpponent(true);
        player.RemoveWizard();
        break;
    case WizardType::CREATE_PIT:
        if (m_board->IsPositionEmpty(possiblePosition)) {
            m_board->CreateHoleAtPosition(possiblePosition);
            m_board->SetShouldResetPositions(true);

            m_board->ChangeTurn();
            player.RemoveWizard();
        }
        else {
            m_board->ReturnCardToDeck(*wizardCard);
        }
        break;
    case WizardType::MOVE_OWN_STACK:
        try {
            if (m_board->GetCardColorAtPosition(possiblePosition) == player.GetColor() && m_board->MoveStackToEmptyPosition(possiblePosition)) {
                m_board->ChangeTurn();
                player.RemoveWizard();
            }
            else {
                m_board->ReturnCardToDeck(*wizardCard);
            }
        }
        catch(std::runtime_error& e){
            m_board->ReturnCardToDeck(*wizardCard);
        }
        break;
    case WizardType::GAIN_ETER_CARD:
    {
        if (!m_board->GetPlayedPositions().contains(possiblePosition)) {

            PlayingCard eterCard = m_board->GenerateEterCard(player.GetColor());
            eterCard.SetBoardPosition(possiblePosition);
            m_board->PushNewCard(eterCard);
            m_board->ChangeTurn();
            player.RemoveWizard();
        }
        else {
            m_board->ReturnCardToDeck(*wizardCard);
        }
        break;
    }
    case WizardType::MOVE_OPPONENT_STACK:
        try {
            if (m_board->GetCardColorAtPosition(possiblePosition) != player.GetColor() && m_board->MoveStackToEmptyPosition(possiblePosition)) {
                m_board->ChangeTurn();
                player.RemoveWizard();
            }
            else {
                m_board->ReturnCardToDeck(*wizardCard);
            }
        }
        catch (std::runtime_error& e) {
            m_board->ReturnCardToDeck(*wizardCard);
        }
        break;
    case WizardType::MOVE_EDGE_ROW:
        if (m_board->MoveEdgeRow(possiblePosition.GetY())) {
            m_board->ChangeTurn();
            player.RemoveWizard();
        }
        else {
            m_board->ReturnCardToDeck(*wizardCard);
        }
        break;
    default:
        break;
    }
}

void Game::PlaySpellCard(Player& player, SpellCard* spellCard, SDL_Rect& renderRect, const Coordinates& possiblePosition) {
    spellCard->SetCoordinates({ renderRect.x, renderRect.y });

    try {
        auto& cards = m_board->GetCardsAtPosition(possiblePosition);
        if (cards.back().IsEter()) {
            m_board->ReturnCardToDeck(*spellCard);
            return;
        }
    }
    catch (std::runtime_error& e) {
        //nothing
    }

    ElementalType spell = spellCard->GetSpell(); //getting the spell type


    switch (spell) {

        case ElementalType::DESTROY: {

            auto enemyPlayedCards = this->m_board->GetPlayerBlue()->GetOrderOfPlayedCards();
            if(this->m_board->GetPlayerBlue() == &player) {
                enemyPlayedCards = this->m_board->GetPlayerRed()->GetOrderOfPlayedCards();
            }
            while(!enemyPlayedCards.empty()) {
                auto card = enemyPlayedCards.top();
                //Validate if the card we want to destroy still exists on the board
                try {
                    auto boardCardDeque = this->m_board->GetCardsAtPosition(card.GetBoardPosition());
                    bool isValid = false;
                    for(auto c : boardCardDeque) {
                        if(c.GetId() == card.GetId()) {
                            isValid = true;
                            break;
                        }
                    }
                    if (isValid) {
                        std::unique_ptr<ExplosionCard> expl = std::make_unique<ExplosionCard>(m_board->GetTableSize());
                        std::vector<std::pair<Coordinates, ExplosionType>> ex;
                        ex.push_back({m_board->GetUnTranslatedPosition(card.GetBoardPosition()), ExplosionType::DELETE});
                        expl->MakeExplosionFromVector(ex);
                        if (m_board->ValidateBoardAfterEffect(expl.get()) && !card.IsEter()) {
                            this->m_board->DeleteCardAtPosition(card.GetBoardPosition());
                            m_board->RemoveSpell(spellCard);
                        }
                        else
                        {
                            m_board->ReturnCardToDeck(*spellCard);
                        }
                        return;
                    }
                    enemyPlayedCards.pop();
                } catch (std::exception& e) {
                    enemyPlayedCards.pop();
                }
            }
            break;
        }

        case ElementalType::FIRE: 
        {
            try {
                Color playerColor = player.GetColor();
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

        case ElementalType::SQUALL: {
            m_board->ReturnCoveredCards();      //returns all covered cards to the players hand
            m_board->RemoveSpell(spellCard);    //then remove the spell card
            m_board->ChangeTurn();              //player loses its turn
            break;
        }

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
                    Coordinates unTranslatedPosition{m_board->GetUnTranslatedPosition(possiblePosition)};
                    std::unique_ptr<ExplosionCard> explosion = std::make_unique<ExplosionCard>(m_board->GetTableSize());
                    std::vector <std::pair<Coordinates,ExplosionType>> ex;
                    ex.push_back({ unTranslatedPosition,ExplosionType::HOLE });
                    explosion->MakeExplosionFromVector(ex);
                    if (m_board->ValidateBoardAfterEffect(explosion.get())) {
                        while (size) {                                   //delete all cards from the game
                            m_board->DeleteCardAtPosition(possiblePosition);
                            --size;
                        }
                        m_board->RemoveSpell(spellCard); //then remove the spell card
                    }
                    else {
                        m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
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
                                 
        case ElementalType::BOULDER: {
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
        }

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
                for (const PlayingCard* card : m_board->GetPlayedCards()) {
                    if (card->GetValue() == cardValue) {
                        ++cardsCount;                                       //counting the cards with equal value
                    }
                }

                if (cardsCount >= 2) {
                    for (const auto& card : m_board->GetPlayedCards()) {                    
                        if (card->GetValue() == cardValue) {     
                            m_board->ReturnTopCardAtPosition(card->GetBoardPosition());        //returning the top cards that are found at each position with equal value
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

        case ElementalType::WHIRLWIND: {
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
        }

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
            if (player.HasPlayedIllusion() == true) {        //can only play mist illusion after playing the regular one
                player.IsPlayingIllusion() = true;
                m_board->RemoveSpell(spellCard); //then remove the spell card
            }
            else
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            break;
        }

        case ElementalType::EARTHQUAKE:
        {
            for (const auto& card : m_board->GetPlayedCards()) {
                if (card->GetValue() == 1) {
                    m_board->ReturnTopCardAtPosition(card->GetBoardPosition());        //returning the top cards that are found at each position with equal value
                }
            }
            m_board->RemoveSpell(spellCard);         //then remove the spell card
            m_board->ChangeTurn();
               
            break;
        }
        
        case ElementalType::TIDE:
        {
            try {
                if (m_selectedStack == nullptr) {
                    m_selectedStack = &m_board->GetCardsAtPosition(possiblePosition); //getting the first stack selected
                    m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                    if (m_selectedStack->back().IsEter())
                        m_selectedStack = nullptr;
                }
                else {
                    GameBoard::DeckType* otherStack = &m_board->GetCardsAtPosition(possiblePosition);
                    if (m_selectedStack != otherStack && !otherStack->back().IsEter()) {
                        std::swap(*m_selectedStack, m_board->GetCardsAtPosition(possiblePosition));         //swapping the stacks
                        m_board->RemoveSpell(spellCard);         //then remove the spell card
                        m_board->ChangeTurn();
                    }
                    else {
                        m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                    }
                }
            }
            catch (const std::runtime_error& error) {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }
        
        case ElementalType::TSUNAMI: {
            if (m_board->SetBlockedRow(possiblePosition.GetY())) {  //checking if there are other possible positions that dont contain Y
                m_board->RemoveSpell(spellCard);         //then remove the spell card
                m_board->ChangeTurn();
            }
            else {
                m_board->ReturnCardToDeck(*spellCard);
            }
            break;
        }

        case ElementalType::WAVE: {
            if (m_board->MoveStackToEmptyPosition(possiblePosition)) {      //if the stack was sucesfully moved
                m_board->SetBoundPosition(possiblePosition);                //we set the bound position             
                m_board->RemoveSpell(spellCard);         //then remove the spell card
            }
            else {
                m_board->ReturnCardToDeck(*spellCard);
            }
            break;
        }

        case ElementalType::FLURRY: {
            if (m_board->Flurry(possiblePosition)) {
                m_board->RemoveSpell(spellCard);         //then remove the spell card
            }
            else {
                m_board->ReturnCardToDeck(*spellCard);
            }
            break;
        }

        case ElementalType::MIRAGE:
        {
            Color playerColor = player.GetColor();
            try {
                PlayingCard& topCard = m_board->GetCardsAtPosition(possiblePosition).back();
                if (topCard.GetColor() == playerColor && topCard.IsIllusion()) {
                    m_board->SetIsPlayingMirage(true);
                    m_board->SetBoundPosition(possiblePosition);
                    m_board->RemoveSpell(spellCard); //then remove the spell card
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

        case ElementalType::BORDERS: {
            m_board->FixBorders(possiblePosition);
            m_board->RemoveSpell(spellCard);
            break;
        }

        case ElementalType::WHIRLPOOL: {
            if (m_board->WhirlPool(possiblePosition)) {
                m_board->RemoveSpell(spellCard); //then remove the spell card
            }
            else {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }

        case ElementalType::HURRICANE: {
            if (m_board->Hurricane(possiblePosition)) {
                m_board->RemoveSpell(spellCard); //then remove the spell card
            }
            else {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }
    }
}

void Game::PlayerTurn(Player& player, SDL_Rect& renderRect, const Coordinates& possiblePosition) {
    Card* pushCard = player.GetGrabbedCard();

    if (pushCard!=nullptr) {
        if (PlayingCard* playingCard = dynamic_cast<PlayingCard*>(pushCard)) {
            PlayRegularCard(player, playingCard, renderRect, possiblePosition);
        }
        else if (SpellCard* spellCard = dynamic_cast<SpellCard*>(pushCard)) {
            if (spellCard) 
                PlaySpellCard(player, spellCard,renderRect,possiblePosition);
        }
        else if (WizardCard* wizardCard = dynamic_cast<WizardCard*>(pushCard)) {
            if (wizardCard) {
                PlayWizardCard(player, wizardCard, renderRect, possiblePosition);
            }
        }
        SaveGame();
    }
    
    //returning cards to deck if they are moved
    for (PlayingCard& card : player.GetCards()) {
        m_board->ReturnCardToDeck(card);
    }
}

void Game::HandleBoardState() {

    if (m_currentState == GameState::TOURNAMENT)
        return;

    //logic for quickMatch
    if(m_board->getPlayingQuickMatch()) {
        Player* blue = m_board->GetPlayerBlue();
        auto blueTime = blue->GetTimeRemaining();
        Player* red = m_board->GetPlayerRed();
        auto redTime = red->GetTimeRemaining();


        m_painter->DrawTimer(blueTime, {300, SCREEN_HEIGHT - 100}, 14);
        m_painter->DrawTimer(redTime, {SCREEN_WIDTH - 300, SCREEN_HEIGHT - 100}, 14);
        if(m_board->IsBluePlayer()) {
            red->setDeltaTime(SDL_GetTicks());
            unsigned int timeDiff = SDL_GetTicks() - blue->GetDeltaTime();
            if(timeDiff >= 1000) {
                //std::cout << "Blue player time ticked down to: " << blueTime << '\n';
                blue->setDeltaTime(SDL_GetTicks());
                blue->setTimeRemaining(blueTime - (timeDiff / 1000));
            }
        } else {
            blue->setDeltaTime(SDL_GetTicks());
            unsigned int timeDiff = SDL_GetTicks() - red->GetDeltaTime();
            if(timeDiff >= 1000) {
                //std::cout << "Red player time ticked down to: " << blueTime << '\n';
                red->setDeltaTime(SDL_GetTicks());
                red->setTimeRemaining(redTime - (timeDiff / 1000));
            }
        }

        if(blue->GetTimeRemaining() <= 0 || red->GetTimeRemaining() <= 0) {
            HandleWin();
        }
    }

    if (m_board->ShouldResetPositions())
        m_board->ResetPossiblePositions();

    m_board->UpdateBoardCenter();

    //bool shouldSave = false;

    //m_painter->DrawButton(shouldSave, { SCREEN_WIDTH - 320, SCREEN_HEIGHT - 180 }, 120, 50, "Save game", 14);

    //if (shouldSave) {
    //    SaveGame();
    //}

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
    DrawPlayersCards(m_board->GetPlayerBlue(), m_board->IsBluePlayer(),m_board->GetPlayerRed());
    DrawPlayersCards(m_board->GetPlayerRed(), !m_board->IsBluePlayer(), m_board->GetPlayerBlue());

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
    for (const auto& possiblePosition : m_board->GetPossiblePositions()) { //positions should be shared ptr
        if (&possiblePosition == nullptr) continue;
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
        else if (possiblePosition.GetX() == m_board->GetFixedX() || possiblePosition.GetY() == m_board->GetFixedY()) {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 0, 0, 255);
        }
        else {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 255, 255);
        }
        SDL_RenderDrawRect(m_painter->GetRenderer(), &renderRect);
    }

    for (const auto& card : m_board->GetPlayedCards()) {
        if (card->IsIllusion())
        {
            if (card->GetColor() == BLUE)
                m_painter->DrawCard(*card, m_board->GetPlayerBlue()->GetIllusionTexture().GetTexture());
            else if (card->GetColor() == RED)
            {
                m_painter->DrawCard(*card, m_board->GetPlayerRed()->GetIllusionTexture().GetTexture());
            }
        }
        else
        {
            m_painter->DrawCard(*card, card->GetTexture()->GetTexture());
        }
    }
}

void Game::HandleCardMovement(Player* player, Card& card) {
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

void Game::DrawPlayersCards(Player* player, bool isPlayersTurn, Player* otherPlayer) {
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
        decltype(auto) wizard = player->GetWizardCard();
        decltype(auto) otherWizard = otherPlayer->GetWizardCard();

        if (wizard) {
            DrawAndHandleCard(player, *wizard);
        }
        if (otherWizard) {
            m_painter->DrawCard(*otherWizard, otherPlayer->GetIllusionTexture().GetTexture());
        }

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
    m_painter->DrawButton(dontExplode, {SCREEN_WIDTH - 600, SCREEN_HEIGHT - 100 }, 150, 50, "Skip explosion!", 14);

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
        int spriteSize = m_board->GetTableSize() == 3 ? 32 : 26;
        for (int i = 0; i < m_board->GetTableSize(); i++) {
            for (int j = 0; j < m_board->GetTableSize(); j++) {
                SDL_Rect spriteRect{ explosionRect.x + 6 + (i * (spriteSize + 6)), explosionRect.y + (j * (spriteSize + 2)), spriteSize, spriteSize };
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
        m_painter->DrawButton(exploded, { SCREEN_WIDTH - 750, SCREEN_HEIGHT - 100 }, 100, 50, "Explode!", 14);

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

void Game::LoadSave()
{
    std::string filename = "save_game.json";

    std::ifstream inFile(filename);
    if (!inFile) {
        std::cerr << "Error: Save file not found!" << std::endl;
        return;
    }

    nlohmann::json save;
    inFile >> save;

    if (save.contains("explosionTurn")) {
        m_explosionTurn = save["explosionTurn"].get<bool>();
    }

    if (save.contains("gameState")) {
        m_currentState = save["gameState"].get<GameState>();
        m_board->GeneratePlayerCards(m_currentState);
    }

    m_board->LoadState(save);

    inFile.close();
    std::cout << "Game loaded successfully!" << std::endl;
}

void Game::SaveGame()
{
    json save;
    
    save["gameState"] = m_currentState;
    save["explosionTurn"] = m_explosionTurn;

    m_board->SaveState(save);

    std::ofstream outFile("save_game.json");

    outFile << save.dump(4);
    outFile.close();
}
