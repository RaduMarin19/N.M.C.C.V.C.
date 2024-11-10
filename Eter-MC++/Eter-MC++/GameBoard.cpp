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
    }
}

bool GameBoard::pushNewCard(const PlayingCard& otherCard)
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
        return false;
    }

    //If there is no card at the position create a new stack and add to it
    if (!m_positions.contains(newCardCoords)) {
        std::stack<PlayingCard> cards;
        cards.emplace(otherCard);
        m_positions.emplace(newCardCoords, cards);
    }
    //Otherwise just add to the existing stack
    else {
        auto it = m_positions.find(newCardCoords);
        if (it->second.top().GetValue() < otherCard.GetValue())
            it->second.emplace(otherCard);
        else if (it->second.top().isIllusion()) {
            m_isBluePlayer = !m_isBluePlayer;
            it->second.top().SetIllussion(false);
            return false;
        }
        else return false;
    }

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
    return true;
}

void GameBoard::returnCardToDeck(PlayingCard& card) {
    card.SetCoordinates(card.GetInitialPosition());
    
}

void GameBoard::setTable(short tableSize)
{
    this->tableSize = tableSize;
}

void GameBoard::setGameMode(const GameMode& mode) {
    this->m_gameMode = mode;
}

void GameBoard::clear() {
    m_positions.clear();
    m_possiblePositions.clear();
    m_possiblePositions.insert({ 0,0 });
    m_playerBlue.reset();
    m_playerRed.reset();
}

void GameBoard::generatePlayerCards(const GameMode &mode) {
    if(mode == GameMode::Training) {
        //Initialize a deck for each player
        std::vector<PlayingCard> PlayingCardsBlue;
        std::vector<PlayingCard> PlayingCardsRed;

        int offsetX = 0;

        for(int j = 0; j < 2; j++)
            for(int i = 1; i <= 3; i++) {
                //Fill each deck with cards
                PlayingCard cardBlue({ coordinatePadding + offsetX, SCREEN_HEIGHT - textureHeight - coordinatePadding }, &m_blueCards[i], i, nextCardId(),BLUE);
                PlayingCard cardRed({ coordinatePadding + offsetX, coordinatePadding }, &m_redCards[i], i, nextCardId(),RED);
                std::cout << "Initialized card with x:"<< coordinatePadding + offsetX <<" y:"<< SCREEN_HEIGHT - textureHeight - coordinatePadding <<"\n";
                cardBlue.GetTexture()->getRect().x = coordinatePadding + offsetX;
                cardBlue.GetTexture()->getRect().y = SCREEN_HEIGHT - textureHeight - coordinatePadding;
                cardRed.GetTexture()->getRect().x = coordinatePadding + offsetX;
                cardRed.GetTexture()->getRect().y = coordinatePadding;
                PlayingCardsBlue.emplace_back(cardBlue);
                PlayingCardsRed.emplace_back(cardRed);
                offsetX += textureWidth;
            }
        PlayingCard cardBlue({ coordinatePadding + offsetX, SCREEN_HEIGHT - textureHeight - coordinatePadding }, &m_blueCards[4], 4, nextCardId(),BLUE);
        PlayingCardsBlue.emplace_back(cardBlue);

        PlayingCard cardRed({ coordinatePadding + offsetX, coordinatePadding }, &m_redCards[4], 4, nextCardId(),RED);
        PlayingCardsRed.emplace_back(cardRed);

        //Initialize the two players with the newly generated decks
        Player playerBlue(PlayingCardsBlue);
        this->m_playerBlue = playerBlue;

        Player playerRed(PlayingCardsRed);
        this->m_playerRed = playerRed;
    } 
    else if (mode == GameMode::Elemental) {

    } 
    else if (mode == GameMode::MageDuel) {

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

