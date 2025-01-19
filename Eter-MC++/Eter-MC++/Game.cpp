#include "Game.h"
#include <thread>

Game::Game()
    : m_currentState(GameState::WELCOME_SCREEN) {
    m_painter = std::make_unique<Graphics>();
    m_board = std::make_unique<GameBoard>(m_painter.get()->GetRenderer());
    m_selectedStack = nullptr;
    m_explosionTurn = false;
    m_quickModeTimer = 0;
    m_bestOf = 0;
    m_isPlayingTournament = false;
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

        while (SDL_PollEvent(&e) != 0)
        {
            m_painter->SetEvent(e);
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
        }

        // Initialize renderer color white for the background
        SDL_SetRenderDrawColor(m_painter->GetRenderer(), 0, 0, 0, 255);

        // Clear screen
        SDL_RenderClear(m_painter->GetRenderer());

        switch (m_currentState) {
            case GameState::WELCOME_SCREEN: {
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
            default:
                break;
        }

        if (m_currentState == GameState::RED_PLAYER_WON || m_currentState == GameState::BLUE_PLAYER_WON || m_currentState == GameState::TIE) {
            HandleWin();
        }
        else if (m_currentState == GameState::TRAINING_MODE || m_currentState == GameState::ELEMENTAL_BATTLE ||
            m_currentState == GameState::MAGE_DUEL ||
            m_currentState == GameState::MAGE_ELEMENTAL) {
            HandleBoardState();
            }

        // Update screen
        SDL_RenderPresent(m_painter->GetRenderer());

        //Get total elapsed time since we started to draw the frame, if less than the target frame time, sleep
        unsigned int frameTime = SDL_GetTicks() - frameStart;
        if(TARGET_FRAME_TIME > frameTime) {
            SDL_Delay(TARGET_FRAME_TIME - frameTime);
        }
        m_painter->ResetEvent();
    }
}

void Game::HandleTournamentSelection() {
    m_painter->DrawTournamentModeSelection(m_currentState);
    m_nextRoundState = m_currentState;
    m_bestOf = 5;

    if (m_currentState != GameState::TOURNAMENT) {
        if (m_currentState == GameState::TRAINING_MODE)
            m_board->SetTable(3);
        else
            m_board->SetTable(4);

        m_board->GeneratePlayerCards(m_currentState);
        m_board->InitializeExplosion();
        this->m_isPlayingTournament = true;
        m_miniArena.resize(m_board->GetTableSize());
        for(int i = 0; i < m_board->GetTableSize(); i++) {
            m_miniArena[i].resize(m_board->GetTableSize(), nullptr);
        }

        if (m_board->GetPlayingQuickMatch()) {
            m_board->GetPlayerBlue()->SetDeltaTime(SDL_GetTicks());
            m_board->GetPlayerRed()->SetDeltaTime(SDL_GetTicks());
            m_board->GetPlayerBlue()->SetTimeRemaining(m_quickModeTimer);
            m_board->GetPlayerRed()->SetTimeRemaining(m_quickModeTimer);
        }
    }
}

void Game::HandleQuickModeSelection() { //// QUICK MODE
    m_painter->DrawQuickModeSelection(m_currentState, m_quickModeTimer);

    if (m_currentState != GameState::TOURNAMENT) {
        if (m_currentState == GameState::TRAINING_MODE) {
            m_nextRoundState = m_currentState;
            m_board->SetTable(3);
            m_board->SetPlayingQuickMatch(true);
            m_bestOf = 3;
        }
        else {
            m_board->SetTable(4);
            m_nextRoundState = m_currentState;
            m_bestOf = 5;
            m_board->SetPlayingQuickMatch(true);
        }

        m_board->GeneratePlayerCards(m_currentState);
        m_board->InitializeExplosion();
        m_board->GetPlayerBlue()->SetDeltaTime(SDL_GetTicks());
        m_board->GetPlayerRed()->SetDeltaTime(SDL_GetTicks());
        m_board->GetPlayerBlue()->SetTimeRemaining(m_quickModeTimer);
        m_board->GetPlayerRed()->SetTimeRemaining(m_quickModeTimer);
    }
    else if (m_currentState != GameState::QUICK_MODE) {
        m_board->SetPlayingQuickMatch(true);
        m_nextRoundState = m_currentState;
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

    if (m_currentState == GameState::RED_PLAYER_WON)
    {
        message = "RED player WON";
        if(incrementWin)
        {
            m_board->IncreaseRoundsWon(GameState::RED_PLAYER_WON);
            if(m_isPlayingTournament) {
                const auto redPlayerCards = m_board->GetPlayerRed()->GetOrderOfPlayedCards();
                if(redPlayerCards.size() > 0) {
                    const auto lastCard = redPlayerCards.top();
                    this->AddTokenToArena(lastCard.GetBoardPosition(), 1);
                }
            }
        }
        incrementWin = false;
    }
    else if (m_currentState == GameState::BLUE_PLAYER_WON)
    {
        message = "BLUE player WON";
        if(incrementWin)
        {
            m_board->IncreaseRoundsWon(GameState::BLUE_PLAYER_WON);
            if(m_isPlayingTournament) {
                const auto bluePlayerCards = m_board->GetPlayerBlue()->GetOrderOfPlayedCards();
                if(bluePlayerCards.size() > 0) {
                    const auto lastCard = bluePlayerCards.top();
                    this->AddTokenToArena(lastCard.GetBoardPosition(), 0);
                }
            }
        }
        incrementWin = false;
    }
    else
    {
        message = "TIE!";
    }

    bool isNextRoundButton = false;
    bool isReset = false;

    if(m_board->GetBlueRoundsWon() == m_bestOf / 2 + 1 || m_board->GetRedRoundsWon() == m_bestOf / 2 + 1)
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
    }

    if(m_isPlayingTournament) m_painter->DrawMiniArena(m_miniArena);

    if (isReset) {
        m_currentState = GameState::MODE_SELECTION;
        m_board->Clear();
        m_miniArena.clear();
        m_board->SetBlueRoundsWon(0);
        m_board->SetRedRoundsWon(0);
        m_board->SetPlayingQuickMatch(false);
        m_explosionTurn = false;
        incrementWin = true;
        return true;
    }
    else if (isNextRoundButton) {
        m_currentState = m_nextRoundState;
        m_board->ResetRound(m_currentState);
        m_board->GetPlayerBlue()->SetDeltaTime(SDL_GetTicks());
        m_board->GetPlayerRed()->SetDeltaTime(SDL_GetTicks());
        m_board->GetPlayerRed()->SetTimeRemaining(m_quickModeTimer);
        m_board->GetPlayerBlue()->SetTimeRemaining(m_quickModeTimer);
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
std::string Game::ElementToString(ElementalType element)
{
    switch (element) {
    case ElementalType::CONTROLLED_EXPLOSION:
        return "Explozie controlata: Tabla explodeaza! Obs: Conditii speciale in functie de regula jocului.";
    case ElementalType::DESTROY:
        return "Distrugere: Elimina din joc ultima carte jucata de adversar.";
    case ElementalType::FIRE:
        return "Flacari: Intoarce iluzia adversarului cu fata in sus si joaca o carte pe oricare pozitie de pe tabla.";
    case ElementalType::LAVA:
        return "Lava: Toate cartile vizibile cu un numar specific se intorc in mainile proprietarilor.";
    case ElementalType::ASHES:
        return "Din cenusa: Alege o carte proprie eliminata din joc si joac-o imediat.";
    case ElementalType::SPARKS:
        return "Scantei: Alege orice carte proprie acoperita de adversar si joac-o pe alta pozitie.";
    case ElementalType::WHIRLWIND:
        return "Vifor: Intoarce o carte vizibila a oponentului in mana sa.";
    case ElementalType::SQUALL:
        return "Vijelie: Toate cartile acoperite de alte carti se intorc in mainile proprietarilor.";
    case ElementalType::HURRICANE:
        return "Uragan: Shift-eaza un rand complet ocupat cu 1 pozitie in directia dorita.";
    case ElementalType::FLURRY:
        return "Rafala: Muta o carte vizibila pe o pozitie adiacenta cu o carte cu un numar mai mic.";
    case ElementalType::MIRAGE:
        return "Miraj: Inlocuieste propria iluzie plasata cu o alta iluzie. Obs: Disponibil doar daca se joaca cu iluzii.";
    case ElementalType::STORM:
        return "Furtuna: Elimina din joc orice teanc de carti ce contine 2 sau mai multe carti.";
    case ElementalType::TIDE:
        return "Maree: Interschimba pozitiile a 2 teancuri de carti.";
    case ElementalType::MIST:
        return "Ceata: Joaca inca o iluzie. Obs: Un jucator nu poate avea 2 iluzii simultan pe tabla.";
    case ElementalType::WAVE:
        return "Val: Muta un teanc pe o pozitie adiacenta goala si joaca o carte pe noua pozitie.";
    case ElementalType::WHIRLPOOL:
        return "Vartej de apa: Muta 2 carti de pe acelasi rand pe un spatiu gol. Cartea mai mare se pune deasupra.";
    case ElementalType::TSUNAMI:
        return "Tsunami: Blocheaza un rand pentru urmatoarea tura a adversarului.";
    case ElementalType::WATERFALL:
        return "Cascada: Teancurile de pe un rand \"cad\" spre o margine, formand un teanc.";
    case ElementalType::SUPPORT:
        return "Sprijin: Valoarea unei carti proprii 1/2/3 creste cu 1.";
    case ElementalType::EARTHQUAKE:
        return "Cutremur: Elimina de pe tabla toate cartile vizibile cu numarul 1.";
    case ElementalType::SHATTER:
        return "Sfarama: Valoarea unei carti a adversarului 2/3/4 scade cu 1.";
    case ElementalType::BORDERS:
        return "Granite: Plaseaza o carte neutra pentru a defini o granita a tablei.";
    case ElementalType::AVALANCHE:
        return "Avalansa: Shift-eaza doua teancuri adiacente pe orizontala/verticala cu 1 pozitie.";
    case ElementalType::BOULDER:
        return "Bolovan: Acopera orice iluzie cu o carte, fara a intoarce iluzia.";
    case ElementalType::NO_ELEMENT:
        return "Niciun element.";
    default:
        return "Element necunoscut.";
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

std::string Game::WizardToString(WizardType mage) {
    switch (mage) {
    case WizardType::REMOVE_OPPONENT_CARD:
        return "Remove Opponent Card: Elimina o carte a adversarului de pe tabla care acopera o carte proprie.";
    case WizardType::REMOVE_ROW:
        return "Remove Row: Elimina un intreg rand de carti de pe tabla (minim 3 pozitii, cu o carte proprie vizibila).";
    case WizardType::COVER_OPPONENT_CARD:
        return "Cover Opponent Card: Acopera o carte a oponentului cu o carte proprie de valoare mai mica din mana.";
    case WizardType::CREATE_PIT:
        return "Create Pit: Transforma un spatiu gol de pe tabla intr-o groapa.";
    case WizardType::MOVE_OWN_STACK:
        return "Move Own Stack: Muta un teanc de carti cu propria carte deasupra pe o alta pozitie goala pe tabla.";
    case WizardType::GAIN_ETER_CARD:
        return "Gain Eter Card: Capata o extra carte Eter care se plaseaza imediat pe tabla.";
    case WizardType::MOVE_OPPONENT_STACK:
        return "Move Opponent Stack: Muta un teanc de carti cu cartea adversarului deasupra pe o alta pozitie goala pe tabla.";
    case WizardType::MOVE_EDGE_ROW:
        return "Move Edge Row: Muta un rand de la \"marginea\" tablei pe o alta \"margine\" (minim 3 pozitii).";
    default:
        return "Unknown WizardType.";
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

        case ElementalType::CONTROLLED_EXPLOSION:
        {
            if (m_board->DidExplode()) {
                m_board->InitializeExplosion();
            }
            m_board->Explode();
            m_board->RemoveSpell(spellCard); //then remove the spell card
            m_board->ChangeTurn();
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
                short cardsCount = std::ranges::count_if(m_board->GetPlayedCards(), [cardValue](const PlayingCard* card) {
                    return card->GetValue() == cardValue;
                    });

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

        case ElementalType::SPARKS:
        {
            m_board->ReturnCoveredCards();
            m_board->RemoveSpell(spellCard);
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

        case ElementalType::WATERFALL:
        {
            if (m_board->WaterFall(possiblePosition)) {
                m_board->RemoveSpell(spellCard);
            }
            else {
                m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
            }
            break;
        }

        case ElementalType::AVALANCHE:
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
                        if (m_board->MoveStacksToEmptyPosition(m_selectedStack->back().GetBoardPosition(), otherStack->back().GetBoardPosition())) {
                            m_board->RemoveSpell(spellCard);         //then remove the spell card
                            m_board->ChangeTurn();
                        }
                        else {
                            m_selectedStack = nullptr;
                            m_board->ReturnCardToDeck(*spellCard);   //returning spellcard to its initial position
                        }
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
    }
    SaveGame();

    //returning cards to deck if they are moved
    for (PlayingCard& card : player.GetCards()) {
        m_board->ReturnCardToDeck(card);
    }
}

void Game::HandleBoardState() {

    if (m_currentState == GameState::TOURNAMENT)
        return;

    //logic for quickMatch
    if(m_board->GetPlayingQuickMatch()) {
        Player* blue = m_board->GetPlayerBlue();
        auto blueTime = blue->GetTimeRemaining();
        Player* red = m_board->GetPlayerRed();
        auto redTime = red->GetTimeRemaining();

        m_painter->DrawTimer(blueTime, {300, SCREEN_HEIGHT - 100}, 40);
        m_painter->DrawTimer(redTime, {SCREEN_WIDTH - 300, SCREEN_HEIGHT - 100}, 40);
        if(m_board->IsBluePlayer()) {
            red->SetDeltaTime(SDL_GetTicks());
            unsigned int timeDiff = SDL_GetTicks() - blue->GetDeltaTime();
            if(timeDiff >= 1000) {
                blue->SetDeltaTime(SDL_GetTicks());
                if(blueTime > 0)
                    blue->SetTimeRemaining(blueTime - (timeDiff / 1000));
            }
        } else {
            blue->SetDeltaTime(SDL_GetTicks());
            unsigned int timeDiff = SDL_GetTicks() - red->GetDeltaTime();
            if(timeDiff >= 1000) {
                red->SetDeltaTime(SDL_GetTicks());
                if (redTime > 0)
                    red->SetTimeRemaining(redTime - (timeDiff / 1000));
            }
        }

        if(blue->GetTimeRemaining() <= 0 || red->GetTimeRemaining() <= 0) {
            HandleWin();
        }
    }

    if (m_board->ShouldResetPositions())
        m_board->ResetPossiblePositions();

    m_board->UpdateBoardCenter();

    // Common logic for all modes

    //drawing the play illusion buttons
    if (m_board->GetPlayerBlue()->HasPlayedIllusion() == false && m_board->IsBluePlayer() && m_painter->IsPressingRightClick()) {
        m_board->GetPlayerBlue()->IsPlayingIllusion() = !m_board->GetPlayerBlue()->IsPlayingIllusion();
    }

    if (m_board->GetPlayerRed()->HasPlayedIllusion() == false && !m_board->IsBluePlayer() && m_painter->IsPressingRightClick()) {
        m_board->GetPlayerRed()->IsPlayingIllusion() = !m_board->GetPlayerRed()->IsPlayingIllusion();
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

    if (!m_painter->IsDraggingMouse() && (m_board->GetPlayerRed()->IsGrabbingCard() || m_board->GetPlayerBlue()->IsGrabbingCard())) {
        //std::cout << "Player stopped grabbing a card\n";
        m_board->GetPlayerRed()->SetIsGrabbingCard(false);
        m_board->GetPlayerBlue()->SetIsGrabbingCard(false);

    }
}

void Game::DrawBoard() {
    for (const auto& possiblePosition : m_board->GetPossiblePositions()) { 
        if (&possiblePosition == nullptr) continue;
        SDL_Rect renderRect{};
        renderRect.x = m_board->GetCenterX() - (possiblePosition.GetX() * textureWidth);
        renderRect.y = m_board->GetCenterY() - (possiblePosition.GetY() * textureHeight);
        renderRect.w = textureWidth;
        renderRect.h = textureHeight;

        if (m_painter->IsMouseInRect(renderRect)) {
            SDL_SetRenderDrawColor(m_painter->GetRenderer(), 250, 250, 50, 255);

            if (m_board->IsBluePlayer() && m_board->GetPlayerBlue()->IsGrabbingCard() && !m_painter->IsDraggingMouse()) {
                PlayerTurn(*m_board->GetPlayerBlue(), renderRect, possiblePosition);
            }
            else if (m_board->GetPlayerRed()->IsGrabbingCard() && !m_painter->IsDraggingMouse()) {
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
        if (m_painter->IsDraggingMouse() && !player->IsGrabbingCard()) {
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
        else if (SpellCard* spellCard = dynamic_cast<SpellCard*>(&card)) {
            m_painter->DrawText(ElementToString(spellCard->GetSpell()), m_painter->GetMousePos(), 14, true);
        }
        else if (WizardCard* wizardCard = dynamic_cast<WizardCard*>(&card)) {
            m_painter->DrawText(WizardToString(wizardCard->GetWizard()), m_painter->GetMousePos(), 14, true);
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

void Game::AddTokenToArena(const Coordinates &pos, const unsigned int& team) {
    std::cout << pos.GetX() << " " << pos.GetY() << '\n';
    const auto rawCoords = m_board->GetUnTranslatedPosition(pos);
    std::cout << rawCoords.GetX() << " " << rawCoords.GetY() << '\n';
    m_miniArena[rawCoords.GetX()][rawCoords.GetY()] = m_board->GetTokenCard(team);
    CheckArenaForWin();
}

void Game::CheckArenaForWin() {

    //check rows
    for(int i = 0; i < m_miniArena.size(); i++) {
        short blueTokens = 0, redTokens = 0;
        for(int j = 0; j < m_miniArena[i].size(); j++) {

            if(m_miniArena[i][j] == nullptr) continue;

            if(m_miniArena[i][j]->GetTeam() == 0) blueTokens++;
            if(m_miniArena[i][j]->GetTeam() == 1) redTokens++;
        }
        if(blueTokens == m_miniArena.size()) {
            m_currentState = GameState::BLUE_PLAYER_WON;
            return;
        }
        if(redTokens == m_miniArena.size()) {
            m_currentState = GameState::RED_PLAYER_WON;
            return;
        }
    }

    //check columns
    for(int j = 0; j < m_miniArena.size(); j++) {
        short blueTokens = 0, redTokens = 0;
        for(int i = 0; i < m_miniArena[j].size(); i++) {

            if(m_miniArena[i][j] == nullptr) continue;

            if(m_miniArena[i][j]->GetTeam() == 0) blueTokens++;
            if(m_miniArena[i][j]->GetTeam() == 1) redTokens++;
        }
        if(blueTokens == m_miniArena.size()) {
            m_currentState = GameState::BLUE_PLAYER_WON;
            return;
        }
        if(redTokens == m_miniArena.size()) {
            m_currentState = GameState::RED_PLAYER_WON;
            return;
        }
    }

    //check diagonals
    short blueTokens = 0, redTokens = 0;
    for(int i = 0; i < m_miniArena.size(); i++) {
        if(m_miniArena[i][i] == nullptr) break;

        if(m_miniArena[i][i]->GetTeam() == 0) blueTokens++;
        if(m_miniArena[i][i]->GetTeam() == 1) redTokens++;
    }

    if(blueTokens == m_miniArena.size()) {
        m_currentState = GameState::BLUE_PLAYER_WON;
        return;
    }
    if(redTokens == m_miniArena.size()) {
        m_currentState = GameState::RED_PLAYER_WON;
        return;
    }

    blueTokens = 0, redTokens = 0;
    for(int i = 0; i < m_miniArena.size(); i++) {
        if(m_miniArena[i][m_miniArena.size() - i - 1] == nullptr) break;

        if(m_miniArena[i][m_miniArena.size() - i - 1]->GetTeam() == 0) blueTokens++;
        if(m_miniArena[i][m_miniArena.size() - i - 1]->GetTeam() == 1) redTokens++;
    }

    if(blueTokens == m_miniArena.size()) {
        m_currentState = GameState::BLUE_PLAYER_WON;
        return;
    }
    if(redTokens == m_miniArena.size()) {
        m_currentState = GameState::RED_PLAYER_WON;
        return;
    }
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

    if (save.contains("bestOf")) {
        m_bestOf = save["bestOf"].get<int>();
    }

    if (save.contains("gameState")) {
        m_currentState = save["gameState"].get<GameState>();
        m_board->GeneratePlayerCards(m_currentState);
    }
    
    if (save.contains("nextRoundState")) {
        m_nextRoundState = save["nextRoundState"].get<GameState>();
    }
    
    if (save.contains("isPlayingTournament")) {
        m_isPlayingTournament = save["isPlayingTournament"].get<bool>();
    }

    if (save.contains("miniArena")) {
        const auto size = m_board->GetTableSize();
        m_miniArena.resize(size);

        for (decltype(auto) row : m_miniArena) {
            row.resize(size, nullptr);
        }

        for (const auto& token : save["miniArena"]) {
            int i = token["board_position"][0].get<int>();
            int j = token["board_position"][1].get<int>();

            int team = token["team"].get<unsigned int>();
            
            m_miniArena[i][j] = m_board->GetTokenCard(team);
        }

    }

    m_board->LoadState(save);

    inFile.close();
    std::cout << "Game loaded successfully!" << std::endl;
}

void Game::SaveGame()
{
    json save;
    
    save["gameState"] = m_currentState;
    save["nextRoundState"] = m_nextRoundState;
    save["explosionTurn"] = m_explosionTurn;

    save["isPlayingTournament"] = m_isPlayingTournament;
    save["bestOf"] = m_bestOf;
    save["miniArena"] = nlohmann::json::array();

    for (int i = 0; i < m_miniArena.size(); ++i) {
        for (int j = 0; j < m_miniArena[i].size(); ++j) {
            if (m_miniArena[i][j] != nullptr) {
                save["miniArena"].push_back({
                    {"board_position", {i, j}},
                    {"team", m_miniArena[i][j]->GetTeam()}
                });
            }
        }
    }

    m_board->SaveState(save);
    std::cout << "saved\n";

    std::ofstream outFile("save_game.json");

    outFile << save.dump(4);
    outFile.close();
}
