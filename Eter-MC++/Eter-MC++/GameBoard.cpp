#include "GameBoard.h"
#include <iostream>

GameBoard::GameBoard() : m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 }, m_positions{}, m_possiblePositions{},
m_blueCards{ 0 }, m_redCards{ 0 }, m_explosions{ 0 } {}

void GameBoard::testPossiblePosition(short x, short y)
{
    //If the board is at it's max size and our point is outside the bounds then it is not valid
    if (std::abs(this->m_minX - this->m_maxX) == (GameBoard::tableSize - 1)) {
        if (x < this->m_minX || x > this->m_maxX) {
            std::cout << x << " " << y << " is out of bounds" << std::endl;
            return;
        }
    }

    if (m_holes.find({ x,y }) != m_holes.end()){
        return;  //cannot add a hole as a possible position
    }

    if (std::abs(this->m_minY - this->m_maxY) == (GameBoard::tableSize - 1)) {
        if (y < this->m_minY || y > this->m_maxY) {
            std::cout << x << " " << y << " is out of bounds" << std::endl;
            return;
        }
    }

    //If there is already a card there then no need to add it as a possible position again
    if (!this->m_positions.contains({ x, y }))
        this->m_possiblePositions.emplace(x, y);
}

void GameBoard::checkStatus(GameState &gameState) {
    if (gameState == TRAINING_MODE) {
        
        /// verify rows

        for (int j = m_minY; j <= m_maxY; ++j)
        {
            int redCardsInRow = 0;
            int blueCardsInRow = 0;

            for (int i = m_minX; i <= m_maxX; ++i)
            {
                if (m_positions.find({ i, j }) != m_positions.end())
                {
                    if (m_positions.find({ i, j })->second.top().GetColor() == RED)
                    {
                        redCardsInRow++;
                    }
                    else if (m_positions.find({ i, j })->second.top().GetColor() == BLUE)
                    {
                        blueCardsInRow++;
                    }
                }
            }
            if (redCardsInRow == 3)
            {
                gameState = RED_PLAYER_WON;
                return;
            }
            if (blueCardsInRow == 3)
            {
                gameState = BLUE_PLAYER_WON;
                return;
            }
        }

        /// verify columns

        for (int i = m_minX; i <= m_maxX; ++i)
        {
            int redCardsInColumn = 0;
            int blueCardsInColumn = 0;

            for (int j = m_minY; j <= m_maxY; ++j)
            {
                if (m_positions.find({ i, j }) != m_positions.end())
                {
                    if (m_positions.find({ i, j })->second.top().GetColor() == RED)
                    {
                        redCardsInColumn++;
                    }
                    else if (m_positions.find({ i, j })->second.top().GetColor() == BLUE)
                    {
                        blueCardsInColumn++;
                    }
                }
            }
            if (redCardsInColumn == 3)
            {
                gameState = RED_PLAYER_WON;
                return;
            }
            if (blueCardsInColumn == 3)
            {
                gameState = BLUE_PLAYER_WON;
                return;
            }
        }

        int indexI = m_minX;
        int indexJ = m_minY;

        bool redPlayerWon = true;
        bool bluePlayerWon = true;

        for (int i = 0; i < 3; ++i)
        {
            if (m_positions.find({ indexI + i, indexJ + i }) != m_positions.end())
            {
                if (m_positions.find({ indexI + i, indexJ + i })->second.top().GetColor() == BLUE)
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

        if(redPlayerWon == true)
        {
            gameState = RED_PLAYER_WON;
            return;
        }
        if (bluePlayerWon == true)
        {
            gameState = BLUE_PLAYER_WON;
            return;
        }

        indexI = m_minX;
        indexJ = m_maxY;

        redPlayerWon = true;
        bluePlayerWon = true;

        for (int i = 0; i < 3; ++i)
        {
            if (m_positions.find({ indexI + i, indexJ - i }) != m_positions.end())
            {
                if (m_positions.find({ indexI + i, indexJ - i })->second.top().GetColor() == BLUE)
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
            return;
        }
        if (bluePlayerWon == true)
        {
            gameState = BLUE_PLAYER_WON;
            return;
        }

        int blueScore = 0;
        int redScore = 0;

        //checking to see if the enemy has any more cards
        if ((m_isBluePlayer && m_playerBlue.GetCards().empty()) || (!m_isBluePlayer && m_playerRed.GetCards().empty())) {
            for (auto& [coords, cardStack] : m_positions) {
                if (cardStack.top().GetColor() == BLUE) {
                    if (cardStack.top().isIllusion()) {
                        blueScore += 1;
                    }
                    else blueScore += cardStack.top().GetValue();
                }
                if (cardStack.top().GetColor() == RED) {
                    if (cardStack.top().isIllusion()) {
                        redScore += 1;
                    }
                    else redScore += cardStack.top().GetValue();
                }
            }
            if (redScore > blueScore) {
                gameState = RED_PLAYER_WON;
                return;
            }
            if (redScore < blueScore) {
                gameState = BLUE_PLAYER_WON;
                return;
            }

            if (redScore == blueScore) {
                gameState = TIE;
                return;
            }
        }
    }
}

void GameBoard::rotateExplosionMask() {
    int n = m_explosionMask.size();

    for (int i = 0; i < n / 2; i++) {

        // Consider elements in group of 4 as P1, P2, P3 & P4 in current square
        for (int j = i; j < n - i - 1; j++) {

            // Swap elements in clockwise order
            ExplosionType temp = m_explosionMask[i][j];
            m_explosionMask[i][j] = m_explosionMask[n - 1 - j][i];                 // Move P4 to P1
            m_explosionMask[n - 1 - j][i] = m_explosionMask[n - 1 - i][n - 1 - j]; // Move P3 to P4
            m_explosionMask[n - 1 - i][n - 1 - j] = m_explosionMask[j][n - 1 - i]; // Move P2 to P3
            m_explosionMask[j][n - 1 - i] = temp;                                  // Move P1 to P2
        }
    }
}

void GameBoard::generateRandomExplosion() {

        static bool generated = false;
        if (!generated) {
            short maxEffects = rand() % 3 + 2;
            short numEffects = 0;

            m_explosionMask.fill({ ExplosionType::NONE, ExplosionType::NONE, ExplosionType::NONE });

            while (numEffects <= maxEffects) {
                for (short i = 0; i <= 2; ++i) {
                    for (short j = 0; j <= 2; ++j) {
                        bool generateExplosion = rand() % 2;
                        if (generateExplosion) {
                            ++numEffects;
                            short explosionType = rand() % 21;
                            if (explosionType < 10) {
                                m_explosionMask[i][j] = ExplosionType::RETURN;
                                break;
                            }
                            else if (explosionType > 10) {
                                m_explosionMask[i][j] = ExplosionType::DELETE;
                                break;
                            }
                            else {
                                m_explosionMask[i][j] = ExplosionType::HOLE;
                                break;
                            }

                            // Adaugă un mesaj de debug pentru a verifica valorile
                            std::cout << "Generated explosion at (" << (m_minX + i) << ", " << (m_minY + j) << ")" << std::endl;
                            break;
                        }
                    }
                }
            }
            generated = true;
        }
    


    /*while(numEffects<=maxEffects)
    for (auto it = m_positions.begin(); it != m_positions.end(); ++it) {
        std::cout << it->first.GetX() << " " << it->first.GetY() << "\n";
        bool generateExplosion = rand() % 2;
        if (generateExplosion) {
            numEffects++;
            short explosionType = rand() % 21;
            if (explosionType < 10) {
                if (!it->second.empty()) {
                    auto& card = it->second.top();
                    if (card.GetColor() == BLUE) {
                        returnCardToDeck(card);
                        m_playerBlue.AddCard(card);
                    }
                    else {
                        returnCardToDeck(card);
                        m_playerRed.AddCard(card);
                    }
                    it->second.pop();
                }
                std::cout << "returned card to hand\n";
                if (it->second.empty()) {
                    m_possiblePositions.insert(it->first);
                    m_positions.erase(it);
                }
                break;
            }
            else if (explosionType > 10) {
                std::cout << "removed card from game\n";
                if (!it->second.empty())
                    it->second.pop();
                if (it->second.empty()) {
                    m_possiblePositions.insert(it->first);
                    m_positions.erase(it);
                }
                break;
            }
            else {
                while (!it->second.empty()) {
                    it->second.pop();
                }
                m_holes.insert(it->first);
                m_possiblePositions.erase(it->first);
                m_positions.erase(it);
                std::cout << "hole\n";
                break;
            }
        }
    }*/
}


void GameBoard::explode()
{
    short n = m_explosionMask.size();

    for (short i = 0; i < n; ++i) {
        for (short j = 0; j < n; ++j) {
            short mapX = m_minX + i;
            short mapY = m_minY + j;
            auto it = m_positions.find({ mapX, mapY });
            if (it != m_positions.end()) {
                if (m_explosionMask[i][j] == ExplosionType::RETURN) {

                    if (!it->second.empty()) {
                        auto& card = it->second.top();
                        if (card.GetColor() == BLUE) {
                            returnCardToDeck(card);
                            m_playerBlue.AddCard(card);
                        }
                        else {
                            returnCardToDeck(card);
                            m_playerRed.AddCard(card);
                        }
                        it->second.pop();
                    }
                    std::cout << "returned card to hand\n";
                    if (it->second.empty()) {
                        m_possiblePositions.insert(it->first);
                        m_positions.erase(it);
                    }
                    break;
                }
                else if (m_explosionMask[i][j] == ExplosionType::DELETE) {
                    std::cout << "removed card from game\n";
                    if (!it->second.empty())
                        it->second.pop();
                    if (it->second.empty()) {
                        m_possiblePositions.insert(it->first);
                        m_positions.erase(it);
                    }
                    break;
                }
                else if (m_explosionMask[i][j] == ExplosionType::HOLE) {
                    while (!it->second.empty()) {
                        it->second.pop();
                    }
                    m_holes.insert(it->first);
                    m_possiblePositions.erase(it->first);
                    m_positions.erase(it);
                    std::cout << "hole\n";
                    break;
                }
            }
        }
    }
    m_exploded = true;
}

void GameBoard::printExplosionMask() {
    short n = m_explosionMask.size();
    for (short i = 0; i < n; ++i) {
        for (short j = 0; j < n; ++j) {
            short mapX = m_minX + i;
            short mapY = m_minY + j;

            std::cout << mapX << " " << mapY << " explosion: ";
            switch (m_explosionMask[i][j]) {
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

CardStatus GameBoard::pushNewCard(const PlayingCard& otherCard)
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
        std::stack<PlayingCard> cards;
        cards.emplace(otherCard);
        m_positions.emplace(newCardCoords, cards);
    }
    //Otherwise just add to the existing stack
    else {
        if (!otherCard.isIllusion()) { //if a card is a illusion you cannot add it to an existing stack
            auto it = m_positions.find(newCardCoords); 
            if (it->second.top().GetValue() < otherCard.GetValue()) 
                it->second.emplace(otherCard);
            else if (it->second.top().isIllusion()) {
                m_isBluePlayer = !m_isBluePlayer;
                it->second.top().SetIllussion(false);
                return REMOVED;
            }
            else return IN_HAND;
        }
        else return IN_HAND;
    }

    std::cout << "Card successfully added at: ("
        << newCardCoords.GetX() << ", "
        << newCardCoords.GetY() << ")" << std::endl;

        //Check horizontally for new possible positions
    this->testPossiblePosition(newCardCoords.GetX() - 1, newCardCoords.GetY());
    this->testPossiblePosition(newCardCoords.GetX() + 1, newCardCoords.GetY());

	    //Check vertically for new possible positions
    this->testPossiblePosition(newCardCoords.GetX(), newCardCoords.GetY() - 1);
    this->testPossiblePosition(newCardCoords.GetX(), newCardCoords.GetY() + 1);

	    //Check diagonally for new possible positions
    this->testPossiblePosition(newCardCoords.GetX() - 1, newCardCoords.GetY() - 1);
    this->testPossiblePosition(newCardCoords.GetX() - 1, newCardCoords.GetY() + 1);

    this->testPossiblePosition(newCardCoords.GetX() + 1, newCardCoords.GetY() - 1);
    this->testPossiblePosition(newCardCoords.GetX() + 1, newCardCoords.GetY() + 1);

    //If the board size is at max size, erase all old entries that are out of bounds
    //TODO: this should be it's own function
    if (std::abs(this->m_minX - this->m_maxX) == (GameBoard::tableSize - 1)) {

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

    if (std::abs(this->m_minY - this->m_maxY) == (GameBoard::tableSize - 1)) {
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

void GameBoard::returnCardToDeck(PlayingCard& card) {
    card.SetCoordinates(card.GetInitialPosition());
}

void GameBoard::setTable(short tableSize)
{
    this->tableSize = tableSize;
}

short GameBoard::getTableSize() const
{
	return tableSize;
}

void GameBoard::setGameMode(const GameMode& mode) {
    this->m_gameMode = mode;
}

void GameBoard::clear() {
    m_positions.clear();
    m_possiblePositions.clear();
    m_possiblePositions.insert({ 0,0 });
    m_playerBlue.reset();
    m_exploded = false;
    m_maxX = 0;
    m_minX = 0;
    m_minY = 0;
    m_maxY = 0;
    m_playerRed.reset();
}

bool GameBoard::canUseExplosion() {
    short row = 0;
    short xCount = 0;
    for (int i = m_minX; i <= m_maxX; i++) {
        xCount = 0;
        for (int j = m_minY; j <= m_maxY; j++) {
            if (m_positions.find({ i,j }) != m_positions.end())
                xCount++;
        }
        if (xCount == 3)
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
        if (yCount == 3)
            column++;
    }
    return row+column>1;
}

void GameBoard::generatePlayerCards(const GameMode &mode) {
    if(mode == GameMode::Training) {
        //Initialize a deck for each player
        std::vector<PlayingCard> PlayingCardsBlue;
        std::vector<PlayingCard> PlayingCardsRed;

        int offsetY = 0;

        for(int j = 0; j < 2; j++)
            for(int i = 1; i <= 3; i++) {
                //Fill each deck with cards
                PlayingCard cardBlue({coordinatePadding,  coordinatePadding +offsetY}, &m_blueCards[i], i, nextCardId(),BLUE);
                PlayingCard cardRed({ SCREEN_WIDTH-textureWidth -coordinatePadding, coordinatePadding +offsetY }, &m_redCards[i], i, nextCardId(),RED);
                std::cout << "Initialized card with x:"<< coordinatePadding + offsetY <<" y:"<< SCREEN_HEIGHT - textureHeight - coordinatePadding <<"\n";
                cardBlue.GetTexture()->getRect().x = coordinatePadding ;
                cardBlue.GetTexture()->getRect().y = coordinatePadding+offsetY;
                cardRed.GetTexture()->getRect().x = SCREEN_WIDTH - textureWidth - coordinatePadding;
                cardRed.GetTexture()->getRect().y = coordinatePadding + offsetY;
                PlayingCardsBlue.emplace_back(cardBlue);
                PlayingCardsRed.emplace_back(cardRed);
                offsetY += textureWidth*0.75;
            }
        PlayingCard cardBlue({ coordinatePadding , coordinatePadding + offsetY }, &m_blueCards[4], 4, nextCardId(),BLUE);
        PlayingCardsBlue.emplace_back(cardBlue);

        PlayingCard cardRed({ SCREEN_WIDTH - textureWidth - coordinatePadding , coordinatePadding + offsetY }, &m_redCards[4], 4, nextCardId(),RED);
        PlayingCardsRed.emplace_back(cardRed);

        //Initialize the two players with the newly generated decks
        Player playerBlue(PlayingCardsBlue);
        this->m_playerBlue = playerBlue;

        Player playerRed(PlayingCardsRed);
        this->m_playerRed = playerRed;
    } 
    else if (mode == GameMode::Elemental) {
        std::vector<PlayingCard> PlayingCardsBlue;
        std::vector<PlayingCard> PlayingCardsRed;

        int offsetY = 0;

        PlayingCard cardBlueEter({ coordinatePadding , coordinatePadding + offsetY }, &m_blueCards[0], 5, nextCardId(), BLUE);
        PlayingCardsBlue.emplace_back(cardBlueEter);

        PlayingCard cardRedEter({ SCREEN_WIDTH - textureWidth - coordinatePadding , coordinatePadding + offsetY }, &m_redCards[0], 5, nextCardId(), RED);
        PlayingCardsRed.emplace_back(cardRedEter);
		offsetY += textureWidth * 0.5;
        for (int i = 0; i < 2; i++) {
            //Fill each deck with cards
            PlayingCard cardBlue({ coordinatePadding,  coordinatePadding + offsetY }, &m_blueCards[1], i, nextCardId(), BLUE);
            PlayingCard cardRed({ SCREEN_WIDTH - textureWidth - coordinatePadding, coordinatePadding + offsetY }, &m_redCards[1], i, nextCardId(), RED);
            std::cout << "Initialized card with x:" << coordinatePadding + offsetY << " y:" << SCREEN_HEIGHT - textureHeight - coordinatePadding << "\n";
            cardBlue.GetTexture()->getRect().x = coordinatePadding;
            cardBlue.GetTexture()->getRect().y = coordinatePadding + offsetY;
            cardRed.GetTexture()->getRect().x = SCREEN_WIDTH - textureWidth - coordinatePadding;
            cardRed.GetTexture()->getRect().y = coordinatePadding + offsetY;
            PlayingCardsBlue.emplace_back(cardBlue);
            PlayingCardsRed.emplace_back(cardRed);
            offsetY += textureWidth * 0.5;
        }

        for (int j = 0; j < 3; j++)
            for (int i = 2; i <= 3; i++) {
                //Fill each deck with cards
                PlayingCard cardBlue({ coordinatePadding,  coordinatePadding + offsetY }, &m_blueCards[i], i, nextCardId(), BLUE);
                PlayingCard cardRed({ SCREEN_WIDTH - textureWidth - coordinatePadding, coordinatePadding + offsetY }, &m_redCards[i], i, nextCardId(), RED);
                std::cout << "Initialized card with x:" << coordinatePadding + offsetY << " y:" << SCREEN_HEIGHT - textureHeight - coordinatePadding << "\n";
                cardBlue.GetTexture()->getRect().x = coordinatePadding;
                cardBlue.GetTexture()->getRect().y = coordinatePadding + offsetY;
                cardRed.GetTexture()->getRect().x = SCREEN_WIDTH - textureWidth - coordinatePadding;
                cardRed.GetTexture()->getRect().y = coordinatePadding + offsetY;
                PlayingCardsBlue.emplace_back(cardBlue);
                PlayingCardsRed.emplace_back(cardRed);
                offsetY += textureWidth * 0.5;
            }
        PlayingCard cardBlue({ coordinatePadding , coordinatePadding + offsetY }, &m_blueCards[4], 4, nextCardId(), BLUE);
        PlayingCardsBlue.emplace_back(cardBlue);

        PlayingCard cardRed({ SCREEN_WIDTH - textureWidth - coordinatePadding , coordinatePadding + offsetY }, &m_redCards[4], 4, nextCardId(), RED);
        PlayingCardsRed.emplace_back(cardRed);

        //Initialize the two players with the newly generated decks
        Player playerBlue(PlayingCardsBlue);
        this->m_playerBlue = playerBlue;

        Player playerRed(PlayingCardsRed);
        this->m_playerRed = playerRed;
    } 
    else if (mode == GameMode::MageDuel) {
        std::vector<PlayingCard> PlayingCardsBlue;
        std::vector<PlayingCard> PlayingCardsRed;

        int offsetY = 0;

        PlayingCard cardBlueEter({ coordinatePadding , coordinatePadding + offsetY }, &m_blueCards[0], 5, nextCardId(), BLUE);
        PlayingCardsBlue.emplace_back(cardBlueEter);

        PlayingCard cardRedEter({ SCREEN_WIDTH - textureWidth - coordinatePadding , coordinatePadding + offsetY }, &m_redCards[0], 5, nextCardId(), RED);
        PlayingCardsRed.emplace_back(cardRedEter);
        offsetY += textureWidth * 0.5;
        for (int i = 0; i < 2; i++) {
            //Fill each deck with cards
            PlayingCard cardBlue({ coordinatePadding,  coordinatePadding + offsetY }, &m_blueCards[1], i, nextCardId(), BLUE);
            PlayingCard cardRed({ SCREEN_WIDTH - textureWidth - coordinatePadding, coordinatePadding + offsetY }, &m_redCards[1], i, nextCardId(), RED);
            std::cout << "Initialized card with x:" << coordinatePadding + offsetY << " y:" << SCREEN_HEIGHT - textureHeight - coordinatePadding << "\n";
            cardBlue.GetTexture()->getRect().x = coordinatePadding;
            cardBlue.GetTexture()->getRect().y = coordinatePadding + offsetY;
            cardRed.GetTexture()->getRect().x = SCREEN_WIDTH - textureWidth - coordinatePadding;
            cardRed.GetTexture()->getRect().y = coordinatePadding + offsetY;
            PlayingCardsBlue.emplace_back(cardBlue);
            PlayingCardsRed.emplace_back(cardRed);
            offsetY += textureWidth * 0.5;
        }

        for (int j = 0; j < 3; j++)
            for (int i = 2; i <= 3; i++) {
                //Fill each deck with cards
                PlayingCard cardBlue({ coordinatePadding,  coordinatePadding + offsetY }, &m_blueCards[i], i, nextCardId(), BLUE);
                PlayingCard cardRed({ SCREEN_WIDTH - textureWidth - coordinatePadding, coordinatePadding + offsetY }, &m_redCards[i], i, nextCardId(), RED);
                std::cout << "Initialized card with x:" << coordinatePadding + offsetY << " y:" << SCREEN_HEIGHT - textureHeight - coordinatePadding << "\n";
                cardBlue.GetTexture()->getRect().x = coordinatePadding;
                cardBlue.GetTexture()->getRect().y = coordinatePadding + offsetY;
                cardRed.GetTexture()->getRect().x = SCREEN_WIDTH - textureWidth - coordinatePadding;
                cardRed.GetTexture()->getRect().y = coordinatePadding + offsetY;
                PlayingCardsBlue.emplace_back(cardBlue);
                PlayingCardsRed.emplace_back(cardRed);
                offsetY += textureWidth * 0.5;
            }
        PlayingCard cardBlue({ coordinatePadding , coordinatePadding + offsetY }, &m_blueCards[4], 4, nextCardId(), BLUE);
        PlayingCardsBlue.emplace_back(cardBlue);

        PlayingCard cardRed({ SCREEN_WIDTH - textureWidth - coordinatePadding , coordinatePadding + offsetY }, &m_redCards[4], 4, nextCardId(), RED);
        PlayingCardsRed.emplace_back(cardRed);

        //Initialize the two players with the newly generated decks
        Player playerBlue(PlayingCardsBlue);
        this->m_playerBlue = playerBlue;

        Player playerRed(PlayingCardsRed);
        this->m_playerRed = playerRed;
    } 
    else if (mode == GameMode::Tournament) {

    }
}

bool GameBoard::getCardAtPosition(const Coordinates &coordinates, PlayingCard &card) const {
    auto PlayingCard = this->m_positions.find(coordinates);
    if(PlayingCard != this->m_positions.end()) {
        card = PlayingCard->second.top();
        return true;
    } return false;

}

const std::unordered_set<Coordinates, Coordinates> & GameBoard::GetPossiblePositions() {
    return this->m_possiblePositions;
}

const std::vector<PlayingCard> GameBoard::GetPlayedCards() const {
    std::vector<PlayingCard> playingCards;

    for(auto [k, v] : this->m_positions) {
        playingCards.emplace_back(v.top());
    }
    return playingCards;
}

std::unordered_map<Coordinates, std::stack<PlayingCard>, Coordinates>& GameBoard::GetPlayedPositions()
{
    return m_positions;
}

Player *GameBoard::getPlayerRed() {
    return &m_playerRed;
}

Player *GameBoard::getPlayerBlue() {
    return &m_playerBlue;
}

CardTexture * GameBoard::getBlueIllusionTexture() {
    return this->m_blueCardIllusion;
}

CardTexture * GameBoard::getRedIllusionTexture() {
    return this->m_redCardIllusion;

}

GameBoard::GameBoard(SDL_Renderer* renderer)
{
    //First possible position will always be 0,0
    this->m_possiblePositions.emplace(0, 0);

    //Load all card textures into memory
#if defined linux
    for (int i = 0; i < 5; i++) {
        m_blueCards.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/blue_" + std::to_string(i) + ".jpg");
        m_redCards.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/red_" + std::to_string(i) + ".jpg");
    }
    m_blueCards.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/blue_back.jpg");
    m_redCards.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/red_back.jpg");

    for (int i = 1; i < 9; i++) {
        m_explosions.emplace_back(renderer, "../Eter-MC++/Eter-MC++/Dependencies/textures/explosion_" + std::to_string(i) + ".jpg");
    }

    m_blueCardIllusion = new CardTexture(renderer,"../Eter-MC++/Eter-MC++/Dependencies/textures/blue_back.jpg");
    m_redCardIllusion = new CardTexture(renderer,"../Eter-MC++/Eter-MC++/Dependencies/textures/red_back.jpg");

#else
    for (int i = 0; i < 5; i++) {
        m_blueCards.emplace_back(renderer, "Dependencies/textures/blue_" + std::to_string(i) + ".jpg");
        m_redCards.emplace_back(renderer, "Dependencies/textures/red_" + std::to_string(i) + ".jpg");
    }
    m_blueCards.emplace_back(renderer, "Dependencies/textures/blue_back.jpg");
    m_redCards.emplace_back(renderer, "Dependencies/textures/red_back.jpg");

    for (int i = 1; i < 9; i++) {
        m_explosions.emplace_back(renderer, "Dependencies/textures/explosion_" + std::to_string(i) + ".jpg");
    }

    m_blueCardIllusion = new CardTexture(renderer,"Dependencies/textures/blue_back.jpg");
    m_redCardIllusion = new CardTexture(renderer,"Dependencies/textures/red_back.jpg");
#endif

}

unsigned short GameBoard::nextCardId() {
    return ++GameBoard::m_cardId;
}

void GameBoard::setIsBluePlayer(bool player) {
    this->m_isBluePlayer = player;
}

bool GameBoard::isBluePlayer() {
    return this->m_isBluePlayer;
}

bool GameBoard::didExplode() const
{
    return m_exploded;
}


