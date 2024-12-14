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
            if (m_positions.find({ i, j }) != m_positions.end())
            {
                if (m_positions.find({ i, j })->second.back().GetColor() == RED)
                {
                    stateRow++;
                }
                else if (m_positions.find({ i, j })->second.back().GetColor() == BLUE)
                {
                    stateRow--;
                }
            }
        }
        if (stateRow == m_tableSize)
        {
            gameState = RED_PLAYER_WON;
            return true;
        }
        if (stateRow == -m_tableSize)
        {
            gameState = BLUE_PLAYER_WON;
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
            if (m_positions.find({ i, j }) != m_positions.end())
            {
                if (m_positions.find({ i, j })->second.back().GetColor() == RED)
                {
                    stateCol++;
                }
                else if (m_positions.find({ i, j })->second.back().GetColor() == BLUE)
                {
                    stateCol--;
                }
            }
        }
        if (stateCol == m_tableSize)
        {
            gameState = RED_PLAYER_WON;
            return true;
        }
        if (stateCol == -m_tableSize)
        {
            gameState = BLUE_PLAYER_WON;
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
        if (m_positions.find({ indexI + i, indexJ + i }) != m_positions.end())
        {
            if (m_positions.find({ indexI + i, indexJ + i })->second.back().GetColor() == BLUE)
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
        gameState = RED_PLAYER_WON;
        return true;
    }
    if (bluePlayerWon == true)
    {
        gameState = BLUE_PLAYER_WON;
        return true;
    }

    indexI = m_minX;
    indexJ = m_maxY;

    redPlayerWon = true;
    bluePlayerWon = true;

    for (int i = 0; i < m_tableSize; ++i)
    {
        if (m_positions.find({ indexI + i, indexJ - i }) != m_positions.end())
        {
            if (m_positions.find({ indexI + i, indexJ - i })->second.back().GetColor() == BLUE)
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
        gameState = RED_PLAYER_WON;
        return true;
    }
    if (bluePlayerWon == true)
    {
        gameState = BLUE_PLAYER_WON;
        return true;
    }
    return false;
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
            gameState = RED_PLAYER_WON;
            return true;
        }
        if (redScore < blueScore) {
            gameState = BLUE_PLAYER_WON;
            return true;
        }

        if (redScore == blueScore) {
            gameState = TIE;
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

void GameBoard::DeleteCardAtPosition(const Coordinates& boardPosition) {
    if (m_positions.find(boardPosition) != m_positions.end() && !m_positions[boardPosition].empty())  // temporary: delete after we do explosion checks
    {
        PlayingCard& card = m_positions[boardPosition].back();
        card.SetIllusion(false);    //if its a illusion it loses the property
        if (card.GetColor() == BLUE)
            m_playerBlue.AddRemovedCard(GetCardsAtPosition(boardPosition), card);         //put the card in the deleted stack of each respective player
        else
            m_playerRed.AddRemovedCard(GetCardsAtPosition(boardPosition),card);
        if (m_positions[boardPosition].size() == 0) {
            m_positions.erase(boardPosition);           //if there isnt any card in the deque anymore then remove the position
            //TODO: remove actual possiblePositions and regenerate them.
        }
    }
}

void GameBoard::CreateHoleAtPosition(const Coordinates& boardPosition) {
    if (m_positions.find(boardPosition) != m_positions.end() && !m_positions[boardPosition].empty())  // temporary: delete after we do explosion checks
    {
        m_positions[boardPosition].back().SetIllusion(false);

        for (PlayingCard& card : m_positions[boardPosition]) {
            if (card.GetColor() == BLUE)
                m_playerBlue.AddRemovedCard(GetCardsAtPosition(boardPosition),card);           //put the card in the deleted stack of each respective player
            else
                m_playerRed.AddRemovedCard(GetCardsAtPosition(boardPosition),card);
        }

        m_positions.erase(boardPosition);
        m_possiblePositions.erase(boardPosition);
        m_holes.insert(boardPosition);
    }
}

std::deque<PlayingCard>& GameBoard::GetCardsAtPosition(const Coordinates& position)
{
    if (m_positions.find(position) != m_positions.end())
        return m_positions.at(position);
    else
        throw std::runtime_error("There are no cards at this position\n");
}

void GameBoard::ReturnCardAtPosition(PlayingCard& card) {
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
    if (it != m_positions.end()) {
        if (!it->second.empty()) {
            PlayingCard& card = it->second.back();
            ReturnCardAtPosition(card);

            m_positions[boardPosition].pop_back();
            if (m_positions[boardPosition].size() == 0)      //if there isnt any card in the deque anymore then remove the position
                m_positions.erase(boardPosition);
        }
    }
}

void GameBoard::Explode()
{
    decltype(auto) explosionEffects = m_explosion->GetExplosionMask();
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
        auto& card = cards.back();
        card.SetCoordinates({ static_cast<int>(m_centerX - pos.GetX() * textureWidth), static_cast<int>(m_centerY - pos.GetY() * textureHeight) });
    }
}



unsigned int GameBoard::GetCenterX() const {
    return this->m_centerX;
}

unsigned int GameBoard::GetCenterY() const {
    return this->m_centerY;
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
    this->TestPossiblePosition(boardPosition.GetX() - 1, boardPosition.GetY());
    this->TestPossiblePosition(boardPosition.GetX() + 1, boardPosition.GetY());

    //Check vertically for new possible positions
    this->TestPossiblePosition(boardPosition.GetX(), boardPosition.GetY() - 1);
    this->TestPossiblePosition(boardPosition.GetX(), boardPosition.GetY() + 1);

    //Check diagonally for new possible positions
    this->TestPossiblePosition(boardPosition.GetX() - 1, boardPosition.GetY() - 1);
    this->TestPossiblePosition(boardPosition.GetX() - 1, boardPosition.GetY() + 1);

    this->TestPossiblePosition(boardPosition.GetX() + 1, boardPosition.GetY() - 1);
    this->TestPossiblePosition(boardPosition.GetX() + 1, boardPosition.GetY() + 1);
}

CardStatus GameBoard::PushNewCard(const PlayingCard& otherCard)
{
    Coordinates newCardCoords = otherCard.GetBoardPosition();


    //Update minimum and maximum board coordinates
    if (newCardCoords.GetX() < m_minX) this->m_minX = newCardCoords.GetX();
    if (newCardCoords.GetX() > m_maxX) this->m_maxX = newCardCoords.GetX();

    if (newCardCoords.GetY() < m_minY) this->m_minY = newCardCoords.GetY();
    if (newCardCoords.GetY() > m_maxY) this->m_maxY = newCardCoords.GetY();

    //If the position at which the new card is played is not on the posible positions list, discard it
    if (!m_possiblePositions.contains(newCardCoords)) {
        std::cout << "Not playing card because not a possible position." << newCardCoords.GetX() << " " << newCardCoords.GetY() << "\n";
        return IN_HAND;
    }

    //If there is no card at the position create a new stack and add to it
    if (!m_positions.contains(newCardCoords)) {
        std::deque<PlayingCard> cards;
        cards.emplace_back(otherCard);
        m_positions.emplace(newCardCoords, cards);
    }
    //Otherwise just add to the existing stack
    else {
         if (!otherCard.IsIllusion()&& !otherCard.IsEter()) { //if a card is a illusion you cannot add it to an existing stack
             auto it = m_positions.find(newCardCoords);
             if (it->second.back().GetValue() < otherCard.GetValue()) {
                 it->second.emplace_back(otherCard);
             }
             else if (it->second.back().IsIllusion() && it->second.back().GetColor() != otherCard.GetColor()) {
                 m_isBluePlayer = !m_isBluePlayer;
                 it->second.back().SetIllusion(false);
                 return REMOVED;
             }
             else return IN_HAND;
         }
         else return IN_HAND;;
            
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

void GameBoard::SetGameMode(const GameMode& mode) {
    this->m_gameMode = mode;
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


    for (int i = 0; i < 2; i++) {
        //Fill each deck with cards
        PlayingCard cardBlue({ 0, 0 }, &m_blueCardTextures[1], i, NextCardId(), BLUE);
        PlayingCard cardRed({ 0, 0 }, &m_redCardTextures[1], i, NextCardId(), RED);
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

    PlayingCard cardBlueEter({ 0, 0 }, &m_blueCardTextures[0], 1, NextCardId(), BLUE);
    cardBlueEter.SetEter(true);
    PlayingCardsBlue.emplace_back(std::move(cardBlueEter));

    PlayingCard cardRedEter({ 0, 0 }, &m_redCardTextures[0], 1, NextCardId(), RED);
    cardRedEter.SetEter(true);
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
    int randomIndex1 = 4/*Random::Get(0, 23)*/;
    int randomIndex2 = 11/*Random::Get(0, 23)*/;

    ElementalType spell1 = static_cast<ElementalType>(randomIndex1);
    ElementalType spell2 = static_cast<ElementalType>(randomIndex2);

    std::unique_ptr<SpellCard> cardSpell1= std::make_unique<SpellCard>(SpellCard({ textureWidth + m_playerHandPadding * 3 / 2 , m_playerHandPadding },
        &m_elementalCardTextures[randomIndex1], spell1,
        NextCardId()));

    std::unique_ptr<SpellCard> cardSpell2 = std::make_unique<SpellCard>(SpellCard({ SCREEN_WIDTH - textureWidth * 2 - m_playerHandPadding * 3 / 2 , m_playerHandPadding },
        &m_elementalCardTextures[randomIndex2], spell2,
        NextCardId()));

    m_spells.emplace(std::make_pair(std::move(cardSpell1), std::move(cardSpell2)));

    //Initialize the two players with the newly generated decks
    this->m_playerBlue = Player(PlayingCardsBlue);
    this->m_playerRed = Player(PlayingCardsRed);

    m_playerBlue.SetIllusionTexture(m_blueCardIllusion);
    m_playerRed.SetIllusionTexture(m_redCardIllusion);
}

void GameBoard::GeneratePlayerCards(const GameMode& mode) {
    if (mode == GameMode::Training || mode == GameMode::QuickMode) {
        GenerateTrainingCards();
    }
    else if (mode == GameMode::Elemental || mode == GameMode::MageDuel) {
        GenerateElementalCards();
    }

}

const std::unordered_set<Coordinates, Coordinates::Hash>& GameBoard::GetPossiblePositions() {
    return this->m_possiblePositions;
}

const std::vector<PlayingCard> GameBoard::GetPlayedCards() const {
    std::vector<PlayingCard> playingCards;

    for (auto& [k, v] : this->m_positions) {
        playingCards.emplace_back(v.back());
    }
    return playingCards;
}

std::unordered_map<Coordinates, std::deque<PlayingCard>, Coordinates::Hash>& GameBoard::GetPlayedPositions()
{
    return m_positions;
}

std::unordered_set<Coordinates, Coordinates::Hash>& GameBoard::GetHoles()
{
    return m_holes;
}

Player* GameBoard::GetPlayerRed() {
    return &m_playerRed;
}

Player* GameBoard::GetPlayerBlue() {
    return &m_playerBlue;
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
        m_blueCards.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/blue_" + std::to_string(i) + ".jpg");
        m_redCards.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/red_" + std::to_string(i) + ".jpg");
    }
    m_blueCardTextures.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/blue_back.jpg");
    m_redCardTextures.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/red_back.jpg");

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
    m_blueCardTextures.emplace_back(renderer, "Dependencies/textures/blue_back.jpg");
    m_redCardTextures.emplace_back(renderer, "Dependencies/textures/red_back.jpg");

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
    //First possible position will always be 0,0
    this->m_possiblePositions.emplace(0, 0);

    this->m_centerX = SCREEN_WIDTH / 2 - textureWidth / 2;
    this->m_centerY = SCREEN_HEIGHT / 2 - textureHeight / 2;
    //Load all card textures into memory

    LoadTextures(renderer);
    InitializeExplosion();
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

void GameBoard::UpdateBoardMask()
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (m_positions.find({ m_minX + j, m_minY + i }) != m_positions.end())
                m_boardMask[i][j] = m_positions[{ m_minX + j, m_minY + i }].size();
            else
                m_boardMask[i][j] = 0;
}

uint8_t explodedBoardRemains(std::array<std::array<uint8_t, 3>, 3> explodedBoardMask)
{
    int ct = 0;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (explodedBoardMask[i][j] != 0)
                ++ct;
    return ct;

}

bool GameBoard::ValidateExplosion()
{
    UpdateBoardMask();
    std::cout << "m_boardMask\n";
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            std::cout << (int)m_boardMask[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "m_explosionMask\n";
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            std::cout << (int)m_explosion->GetExplosionMask()[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::array<std::array<uint8_t, 3>, 3> explodedBoardMask = m_boardMask;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (m_explosion->GetExplosionMask()[i][j] != ExplosionType::NONE)
            {
                if (m_explosion->GetExplosionMask()[i][j] == ExplosionType::HOLE)
                    explodedBoardMask[i][j] = 0;
                else if (explodedBoardMask[i][j] > 0)
                    explodedBoardMask[i][j]--;
            }

    if (explodedBoardRemains(explodedBoardMask) <= 1)
        return true;

    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (explodedBoardMask[i][j] != 0)
                return VerifyNeighbours(explodedBoardMask, i, j);
    return false;
}


