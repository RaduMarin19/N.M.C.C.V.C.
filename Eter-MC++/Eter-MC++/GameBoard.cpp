#include "GameBoard.h"

void GameBoard::TestPossiblePosition(short x, short y)
{
    //If the board is at it's max size and our point is outside the bounds then it is not valid
    if (std::abs(this->m_minX - this->m_maxX) == (GameBoard::m_tableSize - 1)) {
        if (x < this->m_minX || x > this->m_maxX) {
            std::cout << x << " " << y << " is out of bounds" << std::endl;
            return;
        }
    }

    if ((m_isMaxXFixed && m_maxX < x)||
        (m_isMinXFixed && m_minX > x)||         //cannot add a position greater than a fixed border
        (m_isMaxYFixed && m_maxY < y)||
        (m_isMinYFixed && m_minY > y))
        return;


    if (m_holes.find({ x,y }) != m_holes.end()) {
        return;  //cannot add a hole as a possible position
    }

    if (std::abs(this->m_minY - this->m_maxY) == (GameBoard::m_tableSize - 1)) {
        if (y < this->m_minY || y > this->m_maxY) {
            std::cout << x << " " << y << " is out of bounds" << std::endl;
            return;
        }
    }

    //If there is already a card there then no need to add it as a possible position again
    if (!this->m_positions.contains({ x, y }))
        this->m_possiblePositions.emplace(x, y);
}

bool GameBoard::CheckRows(GameState& gameState) {
    for (int j = m_minY; j <= m_maxY; ++j)
    {
        short stateRow = 0;

        for (int i = m_minX; i <= m_maxX; ++i)
        {
            Coordinates position{ i,j };
            if (m_positions.find(position) != m_positions.end() && m_positions.at(position).size()>0) 
            {
                if (m_positions.find(position)->second.back().GetColor() == RED)
                {
                    stateRow++;
                }
                else if (m_positions.find(position)->second.back().GetColor() == BLUE)
                {
                    stateRow--;
                }
            }
        }
        if (stateRow == m_tableSize)
        {
            gameState = GameState::RED_PLAYER_WON;
            return true;
        }
        if (stateRow == -m_tableSize)
        {
            gameState = GameState::BLUE_PLAYER_WON;
            return true;
        }
    }
    return false;
}

bool GameBoard::CheckColumns(GameState& gameState)
{
    for (int i = m_minX; i <= m_maxX; ++i)
    {
        short stateCol = 0;

        for (int j = m_minY; j <= m_maxY; ++j)
        {
            Coordinates position{ i,j };
            if (m_positions.find(position) != m_positions.end()&& m_positions.at(position).size()>0)
            {
                if (m_positions.find(position)->second.back().GetColor() == RED)
                {
                    stateCol++;
                }
                else if (m_positions.find(position)->second.back().GetColor() == BLUE)
                {
                    stateCol--;
                }
            }
        }
        if (stateCol == m_tableSize)
        {
            gameState = GameState::RED_PLAYER_WON;
            return true;
        }
        if (stateCol == -m_tableSize)
        {
            gameState = GameState::BLUE_PLAYER_WON;
            return true;
        }
    }
    return false;
}

bool GameBoard::CheckDiagonals(GameState& gameState)
{
    int indexI = m_minX;
    int indexJ = m_minY;

    bool redPlayerWon = true;
    bool bluePlayerWon = true;

    for (int i = 0; i < m_tableSize; ++i)
    {
        Coordinates position{ indexI + i, indexJ + i };
        if (m_positions.find(position) != m_positions.end() && m_positions.at(position).size()>0)
        {
            if (m_positions.find(position)->second.back().GetColor() == BLUE)
                redPlayerWon = false;
            else
                bluePlayerWon = false;
        }
        else
        {
            redPlayerWon = false;
            bluePlayerWon = false;
        }
    }

    if (redPlayerWon == true)
    {
        gameState = GameState::RED_PLAYER_WON;
        return true;
    }
    if (bluePlayerWon == true)
    {
        gameState = GameState::BLUE_PLAYER_WON;
        return true;
    }

    indexI = m_minX;
    indexJ = m_maxY;

    redPlayerWon = true;
    bluePlayerWon = true;

    for (int i = 0; i < m_tableSize; ++i)
    {
        Coordinates position{ indexI + i, indexJ - i };
        if (m_positions.find(position) != m_positions.end() && m_positions.at(position).size() > 0)
        {
            if (m_positions.find(position)->second.back().GetColor() == BLUE)
                redPlayerWon = false;
            else
                bluePlayerWon = false;
        }
        else
        {
            redPlayerWon = false;
            bluePlayerWon = false;
        }
    }

    if (redPlayerWon == true)
    {
        gameState = GameState::RED_PLAYER_WON;
        return true;
    }
    if (bluePlayerWon == true)
    {
        gameState = GameState::BLUE_PLAYER_WON;
        return true;
    }
    return false;
}

void GameBoard::FixBorders(const Coordinates& position) {
    short x = position.GetX();
    short y = position.GetY();

    if (x >= m_maxX) {
        m_maxX = x;
        m_isMaxXFixed = true;
    }
    else if(x<=m_minX){
        m_minX = x;
        m_isMinXFixed = true;
    }

    if (m_maxX == m_minX)
    {
        m_isMinXFixed = false;
        m_isMaxXFixed = false;
    }

    if (y >= m_maxY) {
        m_maxY = y;
        m_isMaxYFixed = true;
    }
    else if(y<=m_minX){
        m_minY = y;
        m_isMinYFixed = true;
    }

    if (m_maxY == m_minY)
    {
        m_isMinYFixed = false;
        m_isMaxYFixed = false;
    }

    m_shouldResetPositions = true;
}

void GameBoard::ResetPossiblePositions() {
    m_possiblePositions.clear();
    m_possiblePositions.insert({ 0,0 });

    for (auto& position : m_positionsToErase) {
        m_positions.erase(position);
    }

    for (auto& [coords, cards] : m_positions) {
        m_possiblePositions.emplace(coords);
        TestPossiblePositions(coords);
    }
    m_shouldResetPositions = false;
}

bool GameBoard::ShouldResetPositions() const
{
    return m_shouldResetPositions;
}

void GameBoard::SetShouldResetPositions(bool shouldResetPositions)
{
    m_shouldResetPositions = shouldResetPositions;
}

bool GameBoard::CheckScore(GameState& gameState) {
    int blueScore = 0;
    int redScore = 0;

    //checking to see if the enemy has any more cards
    if ((m_isBluePlayer && m_playerBlue.GetCards().empty()) || (!m_isBluePlayer && m_playerRed.GetCards().empty())) {
        for (auto& [coords, cardStack] : m_positions) {
            if (cardStack.back().GetColor() == BLUE) {
                if (cardStack.back().IsIllusion()) {
                    blueScore += 1;
                }
                else blueScore += cardStack.back().GetValue();
            }
            if (cardStack.back().GetColor() == RED) {
                if (cardStack.back().IsIllusion()) {
                    redScore += 1;
                }
                else redScore += cardStack.back().GetValue();
            }
        }
        if (redScore > blueScore) {
            gameState = GameState::RED_PLAYER_WON;
            return true;
        }
        if (redScore < blueScore) {
            gameState = GameState::BLUE_PLAYER_WON;
            return true;
        }

        if (redScore == blueScore) {
            gameState = GameState::TIE;
            return true;
        }
    }
    return false;
}

void GameBoard::CheckStatus(GameState& gameState) {

    if (CheckRows(gameState))
        return;

    if (CheckColumns(gameState))
        return;

    if (CheckDiagonals(gameState))
        return;

    if (CheckScore(gameState))
        return;
}

bool GameBoard::ChangeCardValue(PlayingCard& card, short valueChange) {
    if (card.IsEter())      //cannot increase the value on an eter card
        return false;

    if (card.GetValue() + valueChange >= 1 && card.GetValue() + valueChange <= 4) {
        auto& cardTextures = card.GetColor() == BLUE ? m_blueCardTextures : m_redCardTextures;      //getting the reference to respective card textures

        card.SetTexture(&cardTextures[card.GetValue() + valueChange]);  //setting the texture

        card.SetValue(card.GetValue() + valueChange);       //changing the value

        return true;        //returns true if change was succesful
    }
    return false;
}

void GameBoard::ResetCardValue(PlayingCard& card) {
    auto& cardTextures = card.GetColor() == BLUE ? m_blueCardTextures : m_redCardTextures;

    card.SetTexture(&cardTextures[card.GetInitialValue()]);
    card.SetValue(card.GetInitialValue());
}

void GameBoard::DeleteCardAtPosition(const Coordinates& boardPosition) {
    if (m_positions.contains(boardPosition) && !m_positions[boardPosition].empty() && !m_positions[boardPosition].back().IsEter())
    {
        PlayingCard& card = m_positions[boardPosition].back();
        ResetCardValue(card);
        card.SetIllusion(false);    //if its a illusion it loses the property
        if (card.GetColor() == BLUE)
            m_playerBlue.AddRemovedCard(GetCardsAtPosition(boardPosition), card);         //put the card in the deleted stack of each respective player
        else
            m_playerRed.AddRemovedCard(GetCardsAtPosition(boardPosition),card);
        if (m_positions[boardPosition].size() == 0) {
            m_positionsToErase.insert(boardPosition);           //if there isnt any card in the deque anymore then remove the position
            m_shouldResetPositions = true;
        }
    }
}

void GameBoard::CreateHoleAtPosition(const Coordinates& boardPosition) {
    if(m_positions.contains(boardPosition) && !m_positions[boardPosition].back().IsEter())
    {
        m_positions[boardPosition].back().SetIllusion(false);

        for (PlayingCard& card : m_positions[boardPosition]) {
            if (card.GetColor() == BLUE)
                m_playerBlue.AddRemovedCard(GetCardsAtPosition(boardPosition), card);           //put the card in the deleted stack of each respective player
            else
                m_playerRed.AddRemovedCard(GetCardsAtPosition(boardPosition), card);
        }   

        m_positionsToErase.insert(boardPosition);
    }
    m_holes.insert(boardPosition);
}

void GameBoard::SetIsPlayingMirage(bool isPlayingMirage)
{
    m_isPlayingMirage = isPlayingMirage;
}

bool GameBoard::IsPlayingMirage() const
{
    return m_isPlayingMirage;
}

GameBoard::DeckType& GameBoard::GetCardsAtPosition(const Coordinates& position)
{
    if (m_positions.find(position) != m_positions.end())
        return m_positions.at(position);
    else
        throw std::runtime_error("There are no cards at this position\n");
}

void GameBoard::ReturnCardAtPosition(PlayingCard& card) {
    ResetCardValue(card);
    ReturnCardToDeck(card);
    if (card.GetColor() == BLUE) {
        m_playerBlue.AddCard(card);
    }                                   //adding the card to the hand of the respective player
    else if (card.GetColor() == RED) {
        m_playerRed.AddCard(card);
    }
}

void GameBoard::ReturnTopCardAtPosition(const Coordinates& boardPosition) {
    auto it = m_positions.find(boardPosition);
    if (it != m_positions.end()&&it->second.size()>0 && !it->second.back().IsEter()){
          PlayingCard& card = it->second.back();
          ReturnCardAtPosition(card);

          m_positions[boardPosition].pop_back();
          if (m_positions[boardPosition].size() == 0) {     //if there isnt any card in the deque anymore then remove the position
              m_positionsToErase.insert(boardPosition);
              m_shouldResetPositions = true;
          }
    }
}

bool GameBoard::RemoveRow(short column, Color color)
{
    bool foundColor = false;
    int count = 0;

    for (short row = m_minX; row <= m_maxX; ++row) {
        auto it = m_positions.find({ row, column });
        if (it != m_positions.end()) {
            const auto& deck = it->second;
            if (!deck.empty()) {
                if (deck.back().IsEter())
                    return false; //cannot remove a row with a eter card on it
                ++count;
                if (deck.back().GetColor() == color) {
                    foundColor = true;
                }
            }
        }
    }
    if (!foundColor || count < 3) {
        return false;
    }

    std::unique_ptr<ExplosionCard> explosion = std::make_unique<ExplosionCard>(GetTableSize());
    std::vector<std::pair<Coordinates, ExplosionType>> ex;

    for (short row = m_minX; row <= m_maxX; ++row) {
        Coordinates unTranslatedPosition(GetUnTranslatedPosition({ row, column }));
        ex.emplace_back(unTranslatedPosition, ExplosionType::HOLE);
    }

    if (!ValidateBoardAfterEffect(explosion.get())) {
        return false;
    }

    std::vector<Coordinates> deletedPositions;

    for (short row = m_minX; row <= m_maxX; ++row) {
        auto it = m_positions.find({ row, column });
        if (it != m_positions.end()) {
            unsigned short size = it->second.size();
            while (size>0) {
                std::cerr << "deleting x: " << it->first.GetX() << " y: " << it->first.GetY()<<"\n";
                DeleteCardAtPosition(it->first);
                size--;
            }
            m_positions.erase(it->first);
        }
    }

    m_shouldResetPositions = true;
    return true;
}


void GameBoard::Explode()
{
    if(m_validatedExplosion == nullptr) return;

    SetValidatedExplosion(m_explosion.get());

    decltype(auto) explosionEffects = m_validatedExplosion->GetExplosionMask();
    for (int i = 0; i < m_tableSize; ++i) {
        for (int j = 0; j < m_tableSize; ++j) {
            if (explosionEffects[i][j] != ExplosionType::NONE) {
                short translatedX = m_maxX - i;
                short translatedY = m_maxY - j;
                Coordinates position{ translatedX,translatedY };
                std::cout << "expl effect at: " << translatedX << " " << translatedY << std::endl;
                if (explosionEffects[i][j] == ExplosionType::DELETE) {
                    DeleteCardAtPosition(position);
                }
                else if (explosionEffects[i][j] == ExplosionType::HOLE) {
                    CreateHoleAtPosition(position);
                }
                else if (explosionEffects[i][j] == ExplosionType::RETURN) {
                    ReturnTopCardAtPosition(position);
                }
            }

        }
    }

    m_exploded = true;

    m_shouldResetPositions = true;
}

bool GameBoard::Hurricane(const Coordinates& position) {
    short foundDecks = 0;

    short row = position.GetY();
    short XPos = position.GetX();
    
    for (short x = m_minX; x <= m_maxX; ++x) {
        Coordinates temp{ x,row };
        if (!m_positions.contains(temp))
            return false;
        if (m_positions.at(temp).size() == 0 || m_positions.at(temp).back().IsEter())
            return false;
    }

    while (!m_positions[position].empty()) {
        ReturnTopCardAtPosition(position);          //returning the cards on the row that 'falls'
    }

    m_positionsToErase.erase(position);     //removing the position from the positions to be removed(we will use it for moving another stack in it)

    //moving the decks in the desired direction and erasing the tracing position
    if (XPos == m_maxX) {
        for (int i = m_maxX - 1; i >= m_minX; i--)
        {   
            MoveStack({ i,row }, { i + 1,row });           
        }
        m_positions.erase({ m_minX,row });          
        m_minX++;
    }
    else if (XPos == m_minX) {
        for (int i = m_minX + 1; i <= m_maxX; i++)
        {
            MoveStack({ i,row }, { i - 1,row });
        }
        m_positions.erase({ m_maxX,row });
        m_maxX--;
    }
    else return false;

    m_shouldResetPositions = true;

    return true;
}

bool GameBoard::WhirlPool(const Coordinates& position) {
    //TODO: give the player the option to choose which card to place over when the values are = 
    Coordinates left{ position.GetX() + 1, position.GetY() };
    Coordinates right{ position.GetX() - 1, position.GetY() };

    if (!m_positions.contains(left) || !m_positions.contains(right) || m_positions.contains(position))
        return false;

    if (m_positions[left].back().IsEter() || m_positions[right].back().IsEter())
        return false;

    std::unique_ptr<ExplosionCard> explosion = std::make_unique<ExplosionCard>(GetTableSize());
    std::vector <std::pair<Coordinates, ExplosionType>> ex;

    ex.push_back({ GetUnTranslatedPosition(left), ExplosionType::HOLE });
    ex.push_back({ GetUnTranslatedPosition(right), ExplosionType::HOLE });
    ex.push_back({ GetUnTranslatedPosition(position), ExplosionType::ADD });

    explosion->MakeExplosionFromVector(ex);

    if (!ValidateBoardAfterEffect(explosion.get()))
        return false;

    auto& leftCard = m_positions.at(left);
    auto& rightCard = m_positions.at(right);

    if (leftCard.size() != 1 || rightCard.size() != 1)
        return false;
    
    if (leftCard.back().GetValue() < rightCard.back().GetValue()) {
        PlayingCard pushCard = rightCard.back();
        pushCard.SetBoardPosition(left);
        PushNewCard(pushCard);
    }
    else
    {
        PlayingCard pushCard = leftCard.back();
        pushCard.SetBoardPosition(right);
        PushNewCard(pushCard);
    }

    MoveStack(leftCard.back().GetBoardPosition(), position);
    
    rightCard.clear();
    m_positionsToErase.insert(right);

    m_shouldResetPositions = true;

    return true;
}

void GameBoard::UpdateBoardCenter() {

    //Difference between max and min on each axis (how large is the board and in which direction it spans)
    unsigned int offsetX = (m_maxX + m_minX) * -1;
    unsigned int offsetY = (m_maxY + m_minY) * -1;
    //Offset the center by the needed amount so the middle square is always at WIDTH,HEIGHT/2
    m_centerX = (SCREEN_WIDTH / 2 - textureWidth / 2) - ((textureWidth / 2) * offsetX);
    m_centerY = (SCREEN_HEIGHT / 2 - textureHeight / 2) - ((textureHeight / 2) * offsetY);

    //Also shift all played cards by said amount
    for (auto& [pos, cards] : m_positions) {
        if(cards.size()>0)
        {
            auto& card = cards.back();
            card.SetCoordinates({ static_cast<int>(m_centerX - pos.GetX() * textureWidth), static_cast<int>(m_centerY - pos.GetY() * textureHeight) });
        }
    }
}

bool GameBoard::SetBlockedRow(short row)
{
    for (auto& coordinates : m_possiblePositions) {
        if (coordinates.GetY() != row) {
            m_blockedRow = row;
            return true;
        }
    }
    return false;
}

void GameBoard::SetBoundPosition(const Coordinates& position)
{
    m_boundPosition = new Coordinates(position);
}

bool GameBoard::Flurry(const Coordinates& position) {
    Coordinates unTranslatedPosition{GetUnTranslatedPosition(position)};            //getting the untranslated position
    if (!m_positions.contains(position))
        return false;

    ExplosionCard expl(m_tableSize);        
    expl.MakeExplosionFromVector({ {unTranslatedPosition, ExplosionType::HOLE} });
    
    if (ValidateBoardAfterEffect(&expl)) {                  //verifying if we can remove this position

        PlayingCard& topCard = m_positions[position].back();

        auto TryMoveCard = [&](const Coordinates& target) -> bool {             
            if (m_positions.contains(target)) {                                 
                if (m_positions[target].back().GetValue() < topCard.GetValue()) {           //can only place a card over a lower value one
                    m_positions[target].emplace_back(topCard);                  //placing the card
                    m_positions[position].pop_back();                   //removing the card from the old stack
                    if (m_positions[position].empty()) {
                        m_positions.erase(position);            //if it was the last card then remove the positions altogheter from the map
                    }
                    return true;
                }
            }
            return false;
        };

        for (int i = -1; i <= 1; ++i) {
            if (i != 0) {
                Coordinates XPosition{ position.GetX() + i, position.GetY() };
                Coordinates YPosition{ position.GetX(), position.GetY() + i };

                if (!m_positions.contains(position)) {
                    return false;
                }
                if (m_positions[position].back().IsEter()) {
                    return false;
                }

                if (TryMoveCard(XPosition) || TryMoveCard(YPosition)) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool GameBoard::MoveStackToEmptyPosition(const Coordinates& position)
{
    if (m_positions.contains(position) && m_positions[position].back().IsEter())
        return false;

    Coordinates emptyPosition(100,100);
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            Coordinates newPosition{ position.GetX() + i,position.GetY() + j };
            if (i != 0 && j != 0 && !m_positions.contains(newPosition) && m_possiblePositions.contains(position)) {
                emptyPosition = newPosition;
                break;
            }
        }
    }
    if (emptyPosition.GetX() == 100)
        return false;
    MoveStack(position, emptyPosition);
    return true;
}

void GameBoard::MoveStack(const Coordinates& target, const Coordinates& emptyPosition) {
    if (!m_positions.contains(target))
        return;
    if (m_positions[target].back().IsEter())
        return;
    DeckType cards;
    m_positions.emplace(emptyPosition, cards);          //creating dummy stack
    std::swap(GetCardsAtPosition(emptyPosition), GetCardsAtPosition(target));   //swapping the stack
    m_positions.erase(target);      //deleting initial stack position(now empty, swapped with dummy stack)
}

void GameBoard::SetIsPlayingCoverOpponent(bool isPlayingCoverOpponent)
{
    m_isPlayingCoverOpponent = isPlayingCoverOpponent;
}

bool GameBoard::IsPositionEmpty(const Coordinates& position) const
{
    return !m_positions.contains(position);
}

unsigned int GameBoard::GetCenterX() const {
    return this->m_centerX;
}

unsigned int GameBoard::GetCenterY() const {
    return this->m_centerY;
}

unsigned int GameBoard::GetMinX() const
{
    return m_minX;
}

unsigned int GameBoard::GetMinY() const
{
    return m_minY;
}

unsigned int GameBoard::GetMaxX() const
{
    return m_maxX;
}

unsigned int GameBoard::GetMaxY() const
{
    return m_maxY;
}

ExplosionCard* GameBoard::GetExplosion() {
    return m_explosion.get();
}

GameBoard::SpellsType& GameBoard::GetSpells()
{
    return m_spells;
}

void GameBoard::InitializeExplosion() {
    m_explosion = std::make_unique<ExplosionCard>(m_tableSize);
    m_explosion->InitializeExplosionCard();
}

PlayingCard GameBoard::GenerateEterCard(Color color)
{
    PlayingCard eterCard;

    if (color == BLUE) {
        eterCard = { { 0, 0 }, &m_blueCardTextures[0], 1, NextCardId(), BLUE };
    }

    if (color == RED) {
        eterCard = { { 0, 0 }, &m_redCardTextures[0], 1, NextCardId(), RED };
    }

    eterCard.SetEter(true);

    return eterCard;
}

void GameBoard::PrintExplosionMask() {
    short n = m_explosion->GetExplosionMask().size();
    for (short i = 0; i < n; ++i) {
        for (short j = 0; j < n; ++j) {
            short mapX = m_minX + i;
            short mapY = m_minY + j;

            std::cout << mapX << " " << mapY << " explosion: ";
            switch (m_explosion->GetExplosionMask()[i][j]) {
            case ExplosionType::RETURN:
                std::cout << "RETURN\n";
                break;
            case ExplosionType::DELETE:
                std::cout << "DELETE\n";
                break;
            case ExplosionType::HOLE:
                std::cout << "HOLE\n";
                break;
            default:
                std::cout << " No explosion\n";
            }
        }
    }
}

std::unique_ptr<CardTexture>& GameBoard::GetExplosionBoardTexture() {
    return this->m_explosionBoard;
}

CardTexture* GameBoard::GetExplosionSprite(const int& offset) {
    return &this->m_explosionSprites[offset];
}

bool GameBoard::RemoveIllusion(const Coordinates& boardPosition)
{
    auto cards = m_positions.find(boardPosition);
    if (cards != m_positions.end()) {
        auto& card = cards->second.back();
        if (card.IsIllusion())
        {
            card.SetIllusion(false);
            return true;
        }
    }
    return false;
}

void GameBoard::ReturnCoveredCards()
{
    for (auto it = m_positions.begin(); it != m_positions.end(); ++it) {
        auto& deck = it->second;
        while (deck.size() > 1) {
            ReturnCardAtPosition(deck.front());
            deck.pop_front();
        }
    }
}

void GameBoard::RemoveSpell(SpellCard* spell)
{
    if (!m_spells.has_value()) {
        return;
    }

    auto& spells = m_spells.value();

    if (spell == spells.first.get()) {
        spells.first.reset();
    }
    else if (spell == spells.second.get()) {
        spells.second.reset();
    }
    else {
        throw std::out_of_range("Invalid spell ID");
    }

    if (!spells.first && !spells.second) {
        m_spells.reset();
    }
}

void GameBoard::TestPossiblePositions(const Coordinates& boardPosition) {

    this->TestPossiblePosition(boardPosition.GetX() - 1, boardPosition.GetY() - 1);
    this->TestPossiblePosition(boardPosition.GetX() - 1, boardPosition.GetY() + 1);
    this->TestPossiblePosition(boardPosition.GetX() - 1, boardPosition.GetY());
    this->TestPossiblePosition(boardPosition.GetX() + 1, boardPosition.GetY() - 1);
    this->TestPossiblePosition(boardPosition.GetX() + 1, boardPosition.GetY() + 1);
    this->TestPossiblePosition(boardPosition.GetX() + 1, boardPosition.GetY());
    this->TestPossiblePosition(boardPosition.GetX(), boardPosition.GetY() - 1);
    this->TestPossiblePosition(boardPosition.GetX(), boardPosition.GetY() + 1);

}

CardStatus GameBoard::PushNewCard(const PlayingCard& otherCard)
{
    Coordinates newCardCoords = otherCard.GetBoardPosition();

    if (newCardCoords.GetY() == m_blockedRow) {
        return IN_HAND;
    }

    //Update minimum and maximum board coordinates
    if (newCardCoords.GetX() < m_minX&&!m_isMinXFixed) this->m_minX = newCardCoords.GetX();
    if (newCardCoords.GetX() > m_maxX&&!m_isMaxXFixed) this->m_maxX = newCardCoords.GetX();

    if (newCardCoords.GetY() < m_minY&&!m_isMinYFixed) this->m_minY = newCardCoords.GetY();
    if (newCardCoords.GetY() > m_maxY&&!m_isMaxYFixed) this->m_maxY = newCardCoords.GetY();

    //If the position at which the new card is played is not on the posible positions list, discard it
    if (!m_possiblePositions.contains(newCardCoords)) {
        std::cout << "Not playing card because not a possible position." << newCardCoords.GetX() << " " << newCardCoords.GetY() << "\n";
        return IN_HAND;
    }
    
    if (!m_positions.contains(newCardCoords)) {
        if (m_boundPosition != nullptr && *m_boundPosition != newCardCoords) {      //if there is a bound position then only place card on that one
            return IN_HAND;
        }
        DeckType cards;
        cards.emplace_back(otherCard);
        m_positions.emplace(newCardCoords, cards);
        if (m_boundPosition != nullptr) {           //delete it and set it to null if there is any bound position
            delete m_boundPosition;
            m_boundPosition = nullptr;
        }
    }
    //Otherwise just add to the existing stack
    else {
         if ((!otherCard.IsIllusion() && !otherCard.IsEter()) || IsPlayingMirage()) { //if a card is a illusion you cannot add it to an existing stack
             auto it = m_positions.find(newCardCoords);

             if (m_boundPosition != nullptr && *m_boundPosition!=newCardCoords)
             {
                 return IN_HAND;            //if there is a bound position then only place card on that one
             }

             PlayingCard& lastCard = it->second.back();
             
             if (lastCard.IsEter())
             {
                 return IN_HAND;
             }

             if (m_canCoverIllusion) {
                 if (lastCard.IsIllusion())
                 {
                     it->second.emplace_back(otherCard); //placing the card in the stack
                     ResetCardValue(lastCard);
                     m_canCoverIllusion = false;
                     if (IsPlayingMirage()) {
                         lastCard.SetIllusion(false);
                         SetIsPlayingMirage(false);
                         delete m_boundPosition;
                         m_boundPosition = nullptr;
                     }
                 }
                 else return IN_HAND;
             }
             else if (lastCard.IsIllusion()) {  
                 if (lastCard.GetValue() < otherCard.GetValue()) {
                     it->second.emplace_back(otherCard);        //placing the card in the stack
                     lastCard.SetIllusion(false);      //resetting the illusion
                     ResetCardValue(lastCard);
                 }
                 else if (lastCard.GetColor() != otherCard.GetColor()) {      //else, if trying to covver 
                         ChangeTurn();
                         lastCard.SetIllusion(false);
                         return REMOVED;
                 }
             }
             else if (lastCard.GetValue() < otherCard.GetValue()) {
                 it->second.emplace_back(otherCard);
                 ResetCardValue(lastCard);
             }
             else if (lastCard.GetValue() > otherCard.GetValue() && m_isPlayingCoverOpponent) {
                 it->second.emplace_back(otherCard);        //placing the card in the stack
             }
             else
                 return IN_HAND;
             }
         else return IN_HAND;
            
    }

    std::cout << "Card successfully added at: ("
        << newCardCoords.GetX() << ", "
        << newCardCoords.GetY() << ")" << std::endl;

    TestPossiblePositions(newCardCoords);

    //If the board size is at max size, erase all old entries that are out of bounds
    //TODO: this should be it's own function
    if (std::abs(this->m_minX - this->m_maxX) == (GameBoard::m_tableSize - 1)) {

        auto it = this->m_possiblePositions.begin();
        while (it != this->m_possiblePositions.end()) {


            if (it->GetX() < this->m_minX) {
                it = this->m_possiblePositions.erase(it);
                continue;
            }

            if (it == this->m_possiblePositions.end()) break;
            if (it->GetX() > this->m_maxX) {
                it = this->m_possiblePositions.erase(it);
                continue;
            }
            ++it;
        }
    }

    if (std::abs(this->m_minY - this->m_maxY) == (GameBoard::m_tableSize - 1)) {
        auto it = this->m_possiblePositions.begin();
        while (it != this->m_possiblePositions.end()) {


            if (it->GetY() < this->m_minY) {
                it = this->m_possiblePositions.erase(it);
                continue;
            }

            if (it == this->m_possiblePositions.end()) break;
            if (it->GetY() > this->m_maxY) {
                it = this->m_possiblePositions.erase(it);
                continue;
            }
            ++it;
        }
    }
    return ON_BOARD;
}

void GameBoard::ReturnCardToDeck(Card& card) {
    card.SetCoordinates(card.GetInitialPosition());
}

void GameBoard::SetTable(short tableSize)
{
    this->m_tableSize = tableSize;
}

short GameBoard::GetTableSize() const
{
    return m_tableSize;
}

void GameBoard::Clear() {
    m_positions.clear();
    m_possiblePositions.clear();
    m_possiblePositions.insert({ 0,0 });
    m_playerBlue.Reset();
    m_exploded = false;
    m_maxX = 0;
    m_minX = 0;
    m_minY = 0;
    m_maxY = 0;
    m_playerRed.Reset();
    m_explosion->InitializeExplosionCard();
    m_holes.clear();
    m_isBluePlayer = true;
}

void GameBoard::ResetRound(GameState gameState)
{
    Clear();
    if (gameState == GameState::TRAINING_MODE)
        SetTable(3);
    else
        SetTable(4);

    if (gameState == GameState::TRAINING_MODE)
        GenerateTrainingCards();
    if (gameState == GameState::MAGE_DUEL)
        GenerateTrainingCards();
    if (gameState == GameState::ELEMENTAL_BATTLE)
        GenerateTrainingCards();
    if (gameState == GameState::MAGE_ELEMENTAL)
        GenerateTrainingCards();
}

void GameBoard::setPlayingQuickMatch(bool val) {
    m_isPlayingQuickMatch = val;
}

bool GameBoard::getPlayingQuickMatch() const {
    return m_isPlayingQuickMatch;
}

bool GameBoard::CanUseExplosion() {
    if (m_exploded)
        return false;

    short row = 0;
    short xCount = 0;
    for (int i = m_minX; i <= m_maxX; i++) {
        xCount = 0;
        for (int j = m_minY; j <= m_maxY; j++) {
            if (m_positions.find({ i,j }) != m_positions.end())
                xCount++;
        }
        if (xCount == m_tableSize)
            row++;
    }
    short column = 0;
    short yCount = 0;
    for (int j = m_minY; j <= m_maxY; j++) {
        yCount = 0;
        for (int i = m_minX; i <= m_maxX; i++) {
            if (m_positions.find({ i,j }) != m_positions.end())
                yCount++;
        }
        if (yCount == m_tableSize)
            column++;
    }
    return row + column > 1;
}

void GameBoard::GenerateTrainingCards() {
    //Initialize a deck for each player
        std::vector<PlayingCard> PlayingCardsBlue;
        std::vector<PlayingCard> PlayingCardsRed;

        for (int j = 0; j < 2; j++)
            for (int i = 1; i <= 3; i++) {
                //Fill each deck with cards
                PlayingCard cardBlue({ 0,  0 }, &m_blueCardTextures[i], i, NextCardId(), BLUE);
                PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[i], i, NextCardId(), RED);

                PlayingCardsBlue.emplace_back(std::move(cardBlue));
                PlayingCardsRed.emplace_back(std::move(cardRed));
            }
        PlayingCard cardBlue({ 0, 0 }, &m_blueCardTextures[4], 4, NextCardId(), BLUE);
        PlayingCardsBlue.emplace_back(std::move(cardBlue));

        PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[4], 4, NextCardId(), RED);
        PlayingCardsRed.emplace_back(std::move(cardRed));

        //Set how much space we have for our deck, the whole screen - padding top/bottom
        unsigned int totalPadding = m_playerHandPadding * 2;
        unsigned int availableSpace = SCREEN_HEIGHT - totalPadding;
        unsigned int availableSpacePerCard = availableSpace / PlayingCardsBlue.size() + 1;
        unsigned int currentCardOffset = 0;

        //checking if space allocated allows leaving bottom space
        if (availableSpacePerCard * PlayingCardsBlue.size() + m_playerHandPadding > availableSpace) {
            availableSpacePerCard = (availableSpace - m_playerHandPadding) / PlayingCardsBlue.size() + 1;
        }

        std::cout << "At current screen width, each card is " << availableSpacePerCard << " pixels tall\n";
        for (auto& card : PlayingCardsBlue) {
            std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << m_playerHandPadding + currentCardOffset << "\n";
            card.GetTexture()->GetRect().x = m_playerHandPadding;
            card.GetTexture()->GetRect().y = m_playerHandPadding + currentCardOffset;
            card.SetCoordinates({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

            card.SetInitialPosition({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

            currentCardOffset += availableSpacePerCard;
        }

        currentCardOffset = 0;
        for (auto& card : PlayingCardsRed) {
            std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << m_playerHandPadding + currentCardOffset << "\n";
            card.GetTexture()->GetRect().x = (SCREEN_WIDTH - textureWidth) - m_playerHandPadding;
            card.GetTexture()->GetRect().y = m_playerHandPadding + currentCardOffset;
            card.SetCoordinates({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

            card.SetInitialPosition({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

            currentCardOffset += availableSpacePerCard;
        }

        //Initialize the two players with the newly generated decks
        this->m_playerBlue = Player(PlayingCardsBlue);
        this->m_playerRed = Player(PlayingCardsRed);

        m_playerBlue.SetIllusionTexture(m_blueCardIllusion);
        m_playerRed.SetIllusionTexture(m_redCardIllusion);
}

void GameBoard::GenerateElementalCards() {
    //Initialize a deck for each player
    std::vector<PlayingCard> PlayingCardsBlue;
    std::vector<PlayingCard> PlayingCardsRed;

    PlayingCard cardBlue1({ 0, 0 }, &m_blueCardTextures[1], 1, NextCardId(), BLUE);
    PlayingCard cardRed1({ 0, 0 }, &m_redCardTextures[1], 1, NextCardId(), RED);
    PlayingCardsBlue.emplace_back(std::move(cardBlue1));
    PlayingCardsRed.emplace_back(std::move(cardRed1));

    for (int j = 0; j < 3; j++)
        for (int i = 2; i <= 3; i++) {
            //Fill each deck with cards
            PlayingCard cardBlue({ 0,  0 }, &m_blueCardTextures[i], i, NextCardId(), BLUE);
            PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[i], i, NextCardId(), RED);
            PlayingCardsBlue.emplace_back(std::move(cardBlue));
            PlayingCardsRed.emplace_back(std::move(cardRed));
        }
    PlayingCard cardBlue4({ 0, 0 }, &m_blueCardTextures[4], 4, NextCardId(), BLUE);
    PlayingCardsBlue.emplace_back(std::move(cardBlue4));

    PlayingCard cardRed4({ 0, 0 }, &m_redCardTextures[4], 4, NextCardId(), RED);
    PlayingCardsRed.emplace_back(std::move(cardRed4));

    PlayingCard cardBlueEter = GenerateEterCard(BLUE);
    PlayingCardsBlue.emplace_back(std::move(cardBlueEter));

    PlayingCard cardRedEter = GenerateEterCard(RED);
    PlayingCardsRed.emplace_back(std::move(cardRedEter));

    //Set how much space we have for our deck, the whole screen - padding top/bottom
    unsigned int totalPadding = m_playerHandPadding * 2;
    unsigned int availableSpace = SCREEN_HEIGHT - totalPadding;
    unsigned int availableSpacePerCard = availableSpace / PlayingCardsBlue.size() + 1;
    unsigned int currentCardOffset = 0;

    ////checking if space allocated allows leaving bottom space
    if (availableSpacePerCard * PlayingCardsBlue.size() + m_playerHandPadding > availableSpace) {
        availableSpacePerCard = (availableSpace - m_playerHandPadding) / PlayingCardsBlue.size() + 1;
    }

    std::cout << "At current screen height, each card is " << availableSpacePerCard << " pixels tall\n";

    for (auto& card : PlayingCardsBlue) {
        std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << currentCardOffset << "\n";
        card.GetTexture()->GetRect().x = m_playerHandPadding;
        card.GetTexture()->GetRect().y = currentCardOffset;
        card.SetCoordinates({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        card.SetInitialPosition({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        currentCardOffset += availableSpacePerCard;
    }


    currentCardOffset = 0;
    for (auto& card : PlayingCardsRed) {
        std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << m_playerHandPadding + currentCardOffset << "\n";
        card.GetTexture()->GetRect().x = (SCREEN_WIDTH - textureWidth) - m_playerHandPadding;
        card.GetTexture()->GetRect().y = m_playerHandPadding + currentCardOffset;
        card.SetCoordinates({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        card.SetInitialPosition({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        currentCardOffset += availableSpacePerCard;
    }
    int randomIndex1 = 1/*Random::Get(0, 23)*/;
    int randomIndex2 = 19/*Random::Get(0, 23)*/;

    InitializeSpellCards(randomIndex1, randomIndex2,0);

    //Initialize the two players with the newly generated decks
    this->m_playerBlue = Player(PlayingCardsBlue);
    this->m_playerRed = Player(PlayingCardsRed);

    m_playerBlue.SetIllusionTexture(m_blueCardIllusion);
    m_playerRed.SetIllusionTexture(m_redCardIllusion);
}

void GameBoard::GenerateMageDuelCards() {
    std::vector<PlayingCard> PlayingCardsBlue;
    std::vector<PlayingCard> PlayingCardsRed;


    for (int i = 0; i < 2; i++) {
        //Fill each deck with cards
        PlayingCard cardBlue({ 0, 0 }, &m_blueCardTextures[1], 1, NextCardId(), BLUE);
        PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[1], 1, NextCardId(), RED);
        PlayingCardsBlue.emplace_back(std::move(cardBlue));
        PlayingCardsRed.emplace_back(std::move(cardRed));
    }

    for (int j = 0; j < 3; j++)
        for (int i = 2; i <= 3; i++) {
            //Fill each deck with cards
            PlayingCard cardBlue({ 0, 0 }, &m_blueCardTextures[i], i, NextCardId(), BLUE);
            PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[i], i, NextCardId(), RED);
            PlayingCardsBlue.emplace_back(std::move(cardBlue));
            PlayingCardsRed.emplace_back(std::move(cardRed));
        }
    PlayingCard cardBlue({ 0, 0 }, &m_blueCardTextures[4], 4, NextCardId(), BLUE);
    PlayingCardsBlue.emplace_back(std::move(cardBlue));

    PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[4], 4, NextCardId(), RED);
    PlayingCardsRed.emplace_back(std::move(cardRed));

    PlayingCard cardBlueEter = GenerateEterCard(BLUE);
    PlayingCardsBlue.emplace_back(std::move(cardBlueEter));

    PlayingCard cardRedEter = GenerateEterCard(RED);
    PlayingCardsRed.emplace_back(std::move(cardRedEter));

    //Set how much space we have for our deck, the whole screen - padding top/bottom
    unsigned int totalPadding = m_playerHandPadding * 2;
    unsigned int availableSpace = SCREEN_HEIGHT - totalPadding;
    unsigned int availableSpacePerCard = availableSpace / PlayingCardsBlue.size() + 1;
    unsigned int currentCardOffset = 0;

    ////checking if space allocated allows leaving bottom space
    if (availableSpacePerCard * PlayingCardsBlue.size() + m_playerHandPadding > availableSpace) {
        availableSpacePerCard = (availableSpace - m_playerHandPadding) / PlayingCardsBlue.size() + 1;
    }

    std::cout << "At current screen height, each card is " << availableSpacePerCard << " pixels tall\n";

    for (auto& card : PlayingCardsBlue) {
        std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << currentCardOffset << "\n";
        card.GetTexture()->GetRect().x = m_playerHandPadding;
        card.GetTexture()->GetRect().y = currentCardOffset;
        card.SetCoordinates({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        card.SetInitialPosition({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        currentCardOffset += availableSpacePerCard;
    }


    currentCardOffset = 0;
    for (auto& card : PlayingCardsRed) {
        std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << m_playerHandPadding + currentCardOffset << "\n";
        card.GetTexture()->GetRect().x = (SCREEN_WIDTH - textureWidth) - m_playerHandPadding;
        card.GetTexture()->GetRect().y = m_playerHandPadding + currentCardOffset;
        card.SetCoordinates({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        card.SetInitialPosition({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        currentCardOffset += availableSpacePerCard;
    }


    this->m_playerBlue = Player(PlayingCardsBlue);
    this->m_playerRed = Player(PlayingCardsRed);

    int randomIndex1 = 5/*Random::Get(0, 7)*/;
    int randomIndex2 = 7/*Random::Get(0, 7)*/;

    InitializeWizardCards(randomIndex1,randomIndex2);

    m_playerBlue.SetIllusionTexture(m_blueCardIllusion);
    m_playerRed.SetIllusionTexture(m_redCardIllusion);
}

void GameBoard::GenerateMageElementalCards()
{
    std::vector<PlayingCard> PlayingCardsBlue;
    std::vector<PlayingCard> PlayingCardsRed;

    for (int i = 0; i < 2; i++) {
        //Fill each deck with cards
        PlayingCard cardBlue({ 0, 0 }, &m_blueCardTextures[1], 1, NextCardId(), BLUE);
        PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[1], 1, NextCardId(), RED);
        PlayingCardsBlue.emplace_back(std::move(cardBlue));
        PlayingCardsRed.emplace_back(std::move(cardRed));
    }

    for (int j = 0; j < 3; j++)
        for (int i = 2; i <= 3; i++) {
            //Fill each deck with cards
            PlayingCard cardBlue({ 0,  0 }, &m_blueCardTextures[i], i, NextCardId(), BLUE);
            PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[i], i, NextCardId(), RED);
            PlayingCardsBlue.emplace_back(std::move(cardBlue));
            PlayingCardsRed.emplace_back(std::move(cardRed));
        }
    PlayingCard cardBlue({ 0, 0 }, &m_blueCardTextures[4], 4, NextCardId(), BLUE);
    PlayingCardsBlue.emplace_back(std::move(cardBlue));

    PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[4], 4, NextCardId(), RED);
    PlayingCardsRed.emplace_back(std::move(cardRed));

    PlayingCard cardBlueEter = GenerateEterCard(BLUE);
    PlayingCardsBlue.emplace_back(std::move(cardBlueEter));

    PlayingCard cardRedEter = GenerateEterCard(RED);
    PlayingCardsRed.emplace_back(std::move(cardRedEter));

    //Set how much space we have for our deck, the whole screen - padding top/bottom
    unsigned int totalPadding = m_playerHandPadding * 2;
    unsigned int availableSpace = SCREEN_HEIGHT - totalPadding;
    unsigned int availableSpacePerCard = availableSpace / PlayingCardsBlue.size() + 1;
    unsigned int currentCardOffset = 0;

    ////checking if space allocated allows leaving bottom space
    if (availableSpacePerCard * PlayingCardsBlue.size() + m_playerHandPadding > availableSpace) {
        availableSpacePerCard = (availableSpace - m_playerHandPadding) / PlayingCardsBlue.size() + 1;
    }

    std::cout << "At current screen height, each card is " << availableSpacePerCard << " pixels tall\n";

    for (auto& card : PlayingCardsBlue) {
        std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << currentCardOffset << "\n";
        card.GetTexture()->GetRect().x = m_playerHandPadding;
        card.GetTexture()->GetRect().y = currentCardOffset;
        card.SetCoordinates({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        card.SetInitialPosition({ m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        currentCardOffset += availableSpacePerCard;
    }


    currentCardOffset = 0;
    for (auto& card : PlayingCardsRed) {
        std::cout << "Initialized card with x:" << m_playerHandPadding << " y:" << m_playerHandPadding + currentCardOffset << "\n";
        card.GetTexture()->GetRect().x = (SCREEN_WIDTH - textureWidth) - m_playerHandPadding;
        card.GetTexture()->GetRect().y = m_playerHandPadding + currentCardOffset;
        card.SetCoordinates({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        card.SetInitialPosition({ (SCREEN_WIDTH - textureWidth) - m_playerHandPadding, static_cast<int>(m_playerHandPadding + currentCardOffset) });

        currentCardOffset += availableSpacePerCard;
    }


    this->m_playerBlue = Player(PlayingCardsBlue);
    this->m_playerRed = Player(PlayingCardsRed);

    int randomMageIndex1 = 5/*Random::Get(0, 7)*/;
    int randomMageIndex2 = 7/*Random::Get(0, 7)*/;
    
    int randomSpellIndex1 = 5/*Random::Get(0, 7)*/;
    int randomSpellIndex2 = 7/*Random::Get(0, 7)*/;

    InitializeWizardCards(randomMageIndex1, randomMageIndex2);
    InitializeSpellCards(randomSpellIndex1, randomSpellIndex2,150);

    m_playerBlue.SetIllusionTexture(m_blueCardIllusion);
    m_playerRed.SetIllusionTexture(m_redCardIllusion);
}

bool GameBoard::MoveEdgeRow(short row) {
    if (m_minY == m_maxY) return false; //there is only one row

    short occupiedPositions = 0;
    std::vector<Coordinates> positionsToMove;
    for (auto& position : m_positions) {
        if (position.first.GetY() == row) {
            if (position.second.size() == 0 || position.second.back().IsEter())
                return false;
            occupiedPositions += 1;
            positionsToMove.push_back(position.first);
        }   
    }

    if (occupiedPositions < 3)
        return false;

    short newY;

    if (row == m_minY) {
        m_minY++;
        m_maxY++;
        newY = m_maxY;
    }
    else if (row == m_maxY) {    //getting the new y
        m_maxY--;
        m_minY--;
        newY = m_minY;
    }
    else return false;

    for (auto& position : positionsToMove) {
        Coordinates emptyPosition{ position.GetX(),newY};
        MoveStack(position, emptyPosition);
    }
    m_shouldResetPositions = true;

    return true;
}

void GameBoard::InitializeWizard(Player& player, short wizardId) {
    Coordinates position;

    if (player.GetColor() == BLUE)
        position = { textureWidth + m_playerHandPadding * 3 / 2 , m_playerHandPadding };
    else
        position = { SCREEN_WIDTH - textureWidth * 2 - m_playerHandPadding * 3 / 2 , m_playerHandPadding };

    WizardType wizard = static_cast<WizardType>(wizardId);

    player.SetWizard(wizard, &m_mageCardTextures[wizardId],
        position,
        NextCardId());
}

void GameBoard::InitializeWizardCards(short wizardId1, short wizardId2) {
    InitializeWizard(m_playerBlue, wizardId1);

    InitializeWizard(m_playerRed, wizardId2);
}

void GameBoard::InitializeSpellCards(short spellCardId1,short spellCardId2,int offset) {
    std::unique_ptr<SpellCard> cardSpell1, cardSpell2;

    ElementalType spell1 = static_cast<ElementalType>(spellCardId1);
    ElementalType spell2 = static_cast<ElementalType>(spellCardId2);

    if (spellCardId1 != 24)
        cardSpell1 = std::make_unique<SpellCard>(SpellCard({ static_cast<int>(textureWidth + m_playerHandPadding * 3 / 2 + offset) , m_playerHandPadding },
            &m_elementalCardTextures[spellCardId1], spell1,
            NextCardId()));
    if (spellCardId2 != 24)
        cardSpell2 = std::make_unique<SpellCard>(SpellCard({ static_cast<int>(SCREEN_WIDTH - textureWidth * 2 - m_playerHandPadding * 3 / 2 - offset), m_playerHandPadding },
            &m_elementalCardTextures[spellCardId2], spell2,
            NextCardId()));

    m_spells.emplace(std::make_pair(std::move(cardSpell1), std::move(cardSpell2)));
}

void GameBoard::GeneratePlayerCards(GameState& gameState) {
    if (gameState == GameState::TRAINING_MODE) {
        GenerateTrainingCards();
    }
    else if (gameState == GameState::ELEMENTAL_BATTLE) {
        GenerateElementalCards();
    }
    else if (gameState == GameState::MAGE_DUEL) {
        GenerateMageDuelCards();
    }
    else if (gameState == GameState::MAGE_ELEMENTAL) {
        GenerateMageElementalCards();
    }
}

Coordinates GameBoard::GetUnTranslatedPosition(const Coordinates& position) {
    return {m_maxX - position.GetX(), m_maxY - position.GetY()};
}

const GameBoard::GamePositions& GameBoard::GetPossiblePositions() {
    return this->m_possiblePositions;
}

const std::vector<const PlayingCard*> GameBoard::GetPlayedCards() const {
    std::vector<const PlayingCard*> playingCards;

    for (const auto& [k, v] : this->m_positions) {
        if (!v.empty())
            playingCards.emplace_back(&v.back());
    }
    return playingCards;
}

GameBoard::PlayingBoard& GameBoard::GetPlayedPositions()
{
    return m_positions;
}

GameBoard::GamePositions& GameBoard::GetHoles()
{
    return m_holes;
}

bool GameBoard::ValidateBoardAfterEffect(ExplosionCard *card) {
    auto boardState = this->m_positions;
    //copy our board state to a new object and explode it, then we run our check

    decltype(auto) explosionEffects = card->GetExplosionMask();
    for (int i = 0; i < m_tableSize; ++i) {
        for (int j = 0; j < m_tableSize; ++j) {
            if (explosionEffects[i][j] != ExplosionType::NONE) {
                short translatedX = m_maxX - i;
                short translatedY = m_maxY - j;
                Coordinates position{ translatedX,translatedY };
                if (boardState.contains(position) && boardState[position].back().IsEter())
                    continue;

                if (explosionEffects[i][j] == ExplosionType::DELETE) {
                    if(boardState.contains(position)) {
                        auto it = boardState.find(position);
                        if(!it->second.empty()) {
                            it->second.pop_back();
                            if(it->second.empty())
                                boardState.erase(position);
                        } else boardState.erase(position);
                    }
                }
                else if (explosionEffects[i][j] == ExplosionType::HOLE) {
                    if(boardState.contains(position)) {
                        boardState.erase(position);
                    }
                }
                else if (explosionEffects[i][j] == ExplosionType::RETURN) {
                    if(boardState.contains(position)) {
                        auto it = boardState.find(position);
                        if(!it->second.empty()) {
                            it->second.pop_back();
                            if(it->second.empty())
                                boardState.erase(position);
                        } else boardState.erase(position);
                    }
                }
                else if (explosionEffects[i][j] == ExplosionType::ADD) {
                    PlayingCard dummy;
                    DeckType dummyDeck;
                    dummyDeck.emplace_back(std::move(dummy));
                    boardState.insert({ position , dummyDeck });
                }
            }
        }
    }
    if (boardState.size() == 1)  //edge case where looking for neighbours wouldnt work
        return true;

    for(auto& [k, v] : boardState) {
        int curX = k.GetX();
        int curY = k.GetY();
        int nrNeighbours = 0;
        //check current place for neighbours, if it has none then it is invalidated by our explosion;
        for(int i = -1; i<= 1; ++i) {
            for(int j = -1; j<=1; ++j) {
                if(i == 0 && j == 0) continue;
                if(boardState.contains({curX + i, curY + j})) {
                    if(!boardState.find({curX + i, curY + j})->second.empty()) {
                        nrNeighbours++;
                    }
                }
            }
        }

        std::cout << curX << " " << curY << " " << nrNeighbours << "\n";
        if(nrNeighbours == 0) {
            return false;
        }
    }
    return true;
}

void GameBoard::SetValidatedExplosion(ExplosionCard *card) {

    std::vector<std::vector<ExplosionType>> mask = card->GetExplosionMask();
    using tmp = std::vector<std::pair<Coordinates, ExplosionType>>;
    tmp positions;
    for(int i = 0; i < m_tableSize; ++i) {
        for(int j = 0; j < m_tableSize; ++j) {
            if(mask[i][j] != ExplosionType::NONE) {
                std::pair<Coordinates, ExplosionType> obj;
                obj.first = {i, j};
                obj.second = mask[i][j];
                positions.push_back(obj);
            }
        }
    }
    int effectsSize = pow(2, positions.size());
    std::vector<tmp> explosionSubsets;
    for(int i = 0; i < effectsSize; ++i) {
        tmp subset;
        int copy = i;
        int curBit = 0;
        while(copy) {
            if(copy % 2) subset.push_back(positions[curBit]);
            copy /= 2;
            curBit += 1;
        }
        explosionSubsets.emplace_back(subset);
    }
    std::sort(explosionSubsets.begin(), explosionSubsets.end(), [](tmp obj1, tmp obj2){return obj1.size() > obj2.size();});
    for(const auto& subset : explosionSubsets) {
        ExplosionCard *tmpCard = new ExplosionCard(m_tableSize);
        tmpCard->MakeExplosionFromVector(subset);
        if(this->ValidateBoardAfterEffect(tmpCard)) {
            this->m_validatedExplosion = std::unique_ptr<ExplosionCard>(tmpCard);
            return;
        } else {
            delete tmpCard;
        }
    }
    this->m_validatedExplosion = nullptr;
}

ExplosionCard * GameBoard::GetValidatedExplosion() {
    return this->m_validatedExplosion.get();
}

Player* GameBoard::GetPlayerRed() {
    return &m_playerRed;
}

Player* GameBoard::GetPlayerBlue() {
    return &m_playerBlue;
}

void GameBoard::IncreaseRoundsWon(GameState state)
{
    if (state == GameState::BLUE_PLAYER_WON)
        m_blueRoundsWon++;
    else if (state == GameState::RED_PLAYER_WON)
        m_redRoundsWon++;
}

short GameBoard::GetBlueRoundsWon() const
{
    return m_blueRoundsWon;
}

short GameBoard::GetRedRoundsWon() const
{
    return m_redRoundsWon;
}

void GameBoard::SetBlueRoundsWon(short value)
{
    m_blueRoundsWon = value;
}

void GameBoard::SetRedRoundsWon(short value)
{
    m_redRoundsWon = value;
}

const CardTexture& GameBoard::GetBlueIllusionTexture() const {
    return *m_blueCardIllusion;
}

const CardTexture& GameBoard::GetRedIllusionTexture() const {
    return *m_redCardIllusion;

}

void GameBoard::LoadTextures(SDL_Renderer* renderer) {
#if defined linux
    for (int i = 0; i < 5; i++) {
        m_blueCardTextures.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/blue_" + std::to_string(i) + ".jpg");
        m_redCardTextures.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/red_" + std::to_string(i) + ".jpg");
    }

    for (int i = 0; i < 3; i++) {
        m_explosionSprites.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/explosionSprite_" + std::to_string(i) + ".png");
    }
    m_explosionBoard = std::make_unique<CardTexture>(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/explosion_blank.jpg");

    for (int i = 0; i < 8; i++) {
        m_mageCardTextures.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/mage_" + std::to_string(i) + ".jpg");
    }

    for (int i = 0; i < 24; i++) {
        m_elementalCardTextures.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/spell_" + std::to_string(i) + ".jpg");
    }

    m_blueCardIllusion = std::make_unique<CardTexture>(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/blue_back.jpg");
    m_redCardIllusion = std::make_unique<CardTexture>(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/red_back.jpg");

#else
    for (int i = 0; i < 5; i++) {
        m_blueCardTextures.emplace_back(renderer, "Dependencies/textures/blue_" + std::to_string(i) + ".jpg");
        m_redCardTextures.emplace_back(renderer, "Dependencies/textures/red_" + std::to_string(i) + ".jpg");
    }

    for (int i = 0; i < 3; i++) {
        m_explosionSprites.emplace_back(renderer, "Dependencies/textures/explosionSprite_" + std::to_string(i) + ".png");
    }
    m_explosionBoard = std::make_unique<CardTexture>(renderer, "Dependencies/textures/explosion_blank.jpg");

    for (int i = 0; i < 8; i++) {
        m_mageCardTextures.emplace_back(renderer, "Dependencies/textures/mage_" + std::to_string(i) + ".jpg");
    }

    for (int i = 0; i < 24; i++) {
        m_elementalCardTextures.emplace_back(renderer, "Dependencies/textures/spell_" + std::to_string(i) + ".jpg");
    }

    m_blueCardIllusion = std::make_shared<CardTexture>(renderer, "Dependencies/textures/blue_back.jpg");
    m_redCardIllusion = std::make_shared<CardTexture>(renderer, "Dependencies/textures/red_back.jpg");
#endif
}

GameBoard::GameBoard(SDL_Renderer* renderer)
{
    m_isBluePlayer = true;
    m_canCoverIllusion = false;
    m_isPlayingCoverOpponent = false;
    m_boundPosition = nullptr;
    m_isPlayingMirage = false;
    m_blockedRow = 100;
    m_isMinXFixed = false;
    m_isMaxXFixed = false;
    m_isMinYFixed = false;
    m_isMaxYFixed = false;
    m_isPlayingQuickMatch = false;
    //First possible position will always be 0,0
    this->m_possiblePositions.emplace(0, 0);

    this->m_centerX = SCREEN_WIDTH / 2 - textureWidth / 2;
    this->m_centerY = SCREEN_HEIGHT / 2 - textureHeight / 2;
    //Load all card textures into memory

    LoadTextures(renderer);
}

unsigned short GameBoard::NextCardId() {
    return ++GameBoard::m_cardId;
}

const Color GameBoard::GetCardColorAtPosition(const Coordinates& boardPosition) const
{
    auto it = m_positions.find(boardPosition);
    if (it != m_positions.end())
    {
        return it->second.back().GetColor();
    }
    else {
        throw std::runtime_error("Invalid Position");
    }
}

bool GameBoard::IsBluePlayer() const {
    return this->m_isBluePlayer;
}

void GameBoard::SetCanCoverIllusion(bool canCoverIllusion) {
    m_canCoverIllusion = canCoverIllusion;
}

void GameBoard::ChangeTurn()
{
    m_isBluePlayer = !m_isBluePlayer;
}

bool GameBoard::DidExplode() const
{
    return m_exploded;
}

bool GameBoard::VerifyNeighbours(const std::array<std::array<uint8_t, 3>, 3>& explodedBoardMask, int y, int x)
{
    for (int i = y - 1; i <= y + 1; ++i)
    {
        for (int j = x - 1; j <= x + 1; ++j)
        {
            if (i >= 0 && i <= 2 && j >= 0 && j <= 2 && i != y && j != x)
            {
                if (explodedBoardMask[i][j] != 0)
                    return true;
            }
        }
    }
    return false;
}

short GameBoard::GetFixedX() const
{
    if (m_isMaxXFixed) return m_maxX;
    if (m_isMinXFixed) return m_minX;
    return -16;
}

short GameBoard::GetFixedY() const
{
    if (m_isMaxYFixed) return m_maxY;
    if (m_isMinYFixed) return m_minY;
    return -16;
}

void GameBoard::UpdateBoardMask()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (m_positions.find({ m_minX + j, m_minY + i }) != m_positions.end())
                m_boardMask[i][j] = m_positions[{ m_minX + j, m_minY + i }].size();
            else
                m_boardMask[i][j] = 0;
}

void GameBoard::SaveState(nlohmann::json& json) const {
    json["tableSize"] = m_tableSize;
    json["center"] = { m_centerX, m_centerY };
    json["isBluePlayer"] = m_isBluePlayer;
    json["exploded"] = m_exploded;

    json["blockedRow"] = m_blockedRow;
    json["isMinXFixed"] = m_isMinXFixed;
    json["isMaxXFixed"] = m_isMaxXFixed;
    json["isMinYFixed"] = m_isMinYFixed;
    json["isMaxYFixed"] = m_isMaxYFixed;
    if (m_boundPosition != nullptr) {
        json["boundPosition"] = {
            {"x", m_boundPosition->GetX()},
            {"y", m_boundPosition->GetY()}
        };
    }
    json["minX"] = m_minX;
    json["maxX"] = m_maxX;
    json["minY"] = m_minY;
    json["maxY"] = m_maxY;

    if (m_spells.has_value()) {
        if (m_spells->first.get()) {
            json["firstSpell"] = static_cast<int>(m_spells->first->GetSpell());
        }
        else json["firstSpell"] = 24; // no element

        if (m_spells->second.get()) {
            json["secondSpell"] = static_cast<int>(m_spells->second->GetSpell());
        }
        else json["secondSpell"] = 24;
    }

    //Serialize played positions
    json["playedPositions"] = nlohmann::json::array();
    for (const auto& [position, cards] : m_positions) {
        nlohmann::json positionJson = { {"x", position.GetX()}, {"y", position.GetY()} };

        nlohmann::json cardsJson = nlohmann::json::array();
        for (const auto& card : cards) {
            cardsJson.push_back({
                {"board_position", {card.GetBoardPosition().GetX(), card.GetBoardPosition().GetY()}},
                {"value", card.GetValue()},
                {"initial_value", card.GetInitialValue()},
                {"color", static_cast<int>(card.GetColor())},
                {"is_illusion", card.IsIllusion()},
                {"is_eter", card.IsEter()}
                });
        }

        json["playedPositions"].push_back({ {"position", positionJson}, {"cards", cardsJson} });
    }

    //Serialize possible positions
    json["possiblePositions"] = nlohmann::json::array();
    for (const auto& position : m_possiblePositions) {
        json["possiblePositions"].push_back({ {"x", position.GetX()}, {"y", position.GetY()} });
    }

    //Serialize player states
    auto serializePlayer = [](const Player& player, nlohmann::json& playerJson) {
        playerJson["hasPlayedIllusion"] = player.HasPlayedIllusion();
        playerJson["cards"] = nlohmann::json::array();
        playerJson["removed"] = nlohmann::json::array();

        for (const auto& card : player.GetCards()) {
            playerJson["cards"].push_back({
                {"position", { {"x", card.GetInitialPosition().GetX()}, {"y", card.GetInitialPosition().GetY()} }},
                {"card", {
                    {"value", card.GetValue()},
                    {"initial_value", card.GetInitialValue()},
                    {"color", static_cast<int>(card.GetColor())},
                    {"is_eter", card.IsEter()}
                }}
                });
        }

        for (const auto& card : player.GetRemovedCards()) {
            playerJson["removed"].push_back({
                {"position", { {"x", card.GetInitialPosition().GetX()}, {"y", card.GetInitialPosition().GetY()} }},
                {"card", {
                    {"value", card.GetValue()},
                    {"initial_value", card.GetInitialValue()},
                    {"color", static_cast<int>(card.GetColor())},
                    {"is_eter", card.IsEter()}
                }}
                });
        }
        playerJson["color"] = static_cast<short>(player.GetColor());

        decltype(auto) wizard = player.GetWizardCard().get();

        if (wizard != nullptr) {
            playerJson["mageCard"] = static_cast<short>(wizard->GetWizard());
        }
    };

    if (m_explosion.get()) {
        m_explosion->SaveExplosionToJson(json["explosionCard"]);
    }

    serializePlayer(m_playerBlue, json["PlayerBlue"]);
    serializePlayer(m_playerRed, json["PlayerRed"]);
}

void GameBoard::LoadState(const nlohmann::json& json) {
    if (json.contains("tableSize")) {
        m_tableSize = json["tableSize"].get<short>();
    }

    if (json.contains("center")) {
        m_centerX = json["center"][0].get<unsigned int>();
        m_centerY = json["center"][1].get<unsigned int>();
    }

    if (json.contains("isBluePlayer")) {
        m_isBluePlayer = json["isBluePlayer"].get<bool>();
    }

    if (json.contains("exploded")) {
        m_exploded = json["exploded"].get<bool>();
    }

    // Deserialize played positions
    if (json.contains("playedPositions")) {
        m_positions.clear();

        for (const auto& playedPosition : json["playedPositions"]) {
            Coordinates position;
            position.SetX(playedPosition["position"]["x"].get<int>());
            position.SetY(playedPosition["position"]["y"].get<int>());

            DeckType cards;
            for (const auto& cardJson : playedPosition["cards"]) {
                PlayingCard card;
                card.SetBoardPosition({
                    cardJson["board_position"][0].get<int>(),
                    cardJson["board_position"][1].get<int>()
                    });
                card.SetValue(cardJson["value"].get<short>());
                card.SetInitialValue(cardJson["initial_value"].get<short>());
                card.SetColor(static_cast<Color>(cardJson["color"].get<int>()));
                card.SetIllusion(cardJson["is_illusion"].get<bool>());
                card.SetEter(cardJson["is_eter"].get<bool>());

                // Assign the correct texture based on color
                if (card.GetColor() == BLUE)
                    card.SetTexture(&m_blueCardTextures[card.GetValue()]);
                else
                    card.SetTexture(&m_redCardTextures[card.GetValue()]);

                cards.push_back(card);
            }

            m_positions[position] = std::move(cards);
        }
    }

    // Deserialize possible positions
    if (json.contains("possiblePositions")) {
        m_possiblePositions.clear();

        for (const auto& possiblePosition : json["possiblePositions"]) {
            Coordinates position;
            position.SetX(possiblePosition["x"].get<int>());
            position.SetY(possiblePosition["y"].get<int>());

            m_possiblePositions.insert(position);
        }
    }

    // Deserialize players
    auto deserializePlayer = [&](Player& player, const nlohmann::json& playerJson) {
        if (playerJson.contains("hasPlayedIllusion")&& playerJson["hasPlayedIllusion"].get<bool>()==true) {
            player.SetHasPlayedIllusion();
        }

        if (playerJson.contains("color"))
            player.SetColor(playerJson["color"].get<Color>());

        if (playerJson.contains("cards")) {
            player.ClearCards();
            for (const auto& cardEntry : playerJson["cards"]) {
                PlayingCard card;
                card.SetInitialPosition({
                    cardEntry["position"]["x"].get<int>(),
                    cardEntry["position"]["y"].get<int>()
                    });
                card.SetValue(cardEntry["card"]["value"].get<short>());
                card.SetInitialValue(cardEntry["card"]["initial_value"].get<short>());
                card.SetColor(static_cast<Color>(cardEntry["card"]["color"].get<int>()));
                card.SetEter(cardEntry["card"]["is_eter"].get<bool>());

                // Assign the correct texture based on color
                if (card.GetColor() == BLUE)
                    card.SetTexture(&m_blueCardTextures[card.GetValue()]);
                else
                    card.SetTexture(&m_redCardTextures[card.GetValue()]);

                player.AddCard(card); 
            }
        }

        if (playerJson.contains("removed")) {
            player.ClearRemovedCards(); 
            for (const auto& cardEntry : playerJson["removed"]) {
                PlayingCard card;
                card.SetInitialPosition({
                    cardEntry["position"]["x"].get<int>(),
                    cardEntry["position"]["y"].get<int>()
                    });
                card.SetValue(cardEntry["card"]["value"].get<short>());
                card.SetInitialValue(cardEntry["card"]["initial_value"].get<short>());
                card.SetColor(static_cast<Color>(cardEntry["card"]["color"].get<int>()));
                card.SetEter(cardEntry["card"]["is_eter"].get<bool>());

                // Assign the correct texture based on color
                if (card.GetColor() == BLUE) 
                    card.SetTexture(&m_blueCardTextures[card.GetValue()]);
                else
                    card.SetTexture(&m_redCardTextures[card.GetValue()]);

                player.LoadRemovedCard(card); 
            }
        }

        if (playerJson.contains("mageCard")) {
           InitializeWizard(player, playerJson["mageCard"].get<short>());
        }
    };

    // Deserialize both players
    if (json.contains("PlayerBlue")) {
        deserializePlayer(m_playerBlue, json["PlayerBlue"]);
    }

    if (json.contains("PlayerRed")) {
        deserializePlayer(m_playerRed, json["PlayerRed"]);
    }

    if (json.contains("firstSpell")&&json.contains("secondSpell")) {
        InitializeSpellCards(json["firstSpell"].get<int>(), json["secondSpell"].get<int>(),0);
    }

    if (json.contains("blockedRow")) {
        m_blockedRow = json["blockedRow"].get<short>();
    }

    if (json.contains("isMinXFixed")) {
        m_isMinXFixed = json["isMinXFixed"].get<bool>();
    }

    if (json.contains("explosionCard")) {
        m_explosion.release();
        m_explosion = std::make_unique<ExplosionCard>(m_tableSize);
        m_explosion->LoadExplosionFromJson(json["explosionCard"]);
    }

    if (json.contains("minX")) {
        m_minX = json["minX"].get<short>();
    }

    if (json.contains("maxX")) {
        m_maxX = json["maxX"].get<short>();
    }

    if (json.contains("minY")) {
        m_minY = json["minY"].get<short>();
    }

    if (json.contains("maxY")) {
        m_maxY = json["maxY"].get<short>();
    }

    if (json.contains("isMaxXFixed")) {
        m_isMaxXFixed = json["isMaxXFixed"].get<bool>();
    }

    if (json.contains("isMinYFixed")) {
        m_isMinYFixed = json["isMinYFixed"].get<bool>();
    }

    if (json.contains("isMaxYFixed")) {
        m_isMaxYFixed = json["isMaxYFixed"].get<bool>();
    }

    if (json.contains("boundPosition")) {
        Coordinates position(json["boundPosition"]["x"].get<int>(), json["boundPosition"]["y"].get<int>());
        SetBoundPosition(position);
    }

    //TODO: save explosion
}


