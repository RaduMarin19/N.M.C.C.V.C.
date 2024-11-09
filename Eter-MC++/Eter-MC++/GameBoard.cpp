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

void GameBoard::pushNewCard(const PlayingCard& otherCard)
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
        return;
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
        it->second.emplace(otherCard);
    }

    {
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
    }

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
}

void GameBoard::setTable(short tableSize)
{
    this->tableSize = tableSize;
}

void GameBoard::setGameMode(const GameMode& mode) {
    this->m_gameMode = mode;
}

void GameBoard::generatePlayerCards(const GameMode &mode) {
    if(mode == GameMode::Training) {
        //Initialize a deck for each player
        std::vector<PlayingCard> PlayingCardsBlue;
        std::vector<PlayingCard> PlayingCardsRed;

        int offsetX = 0;

        for(int i = 0; i < 2; i++)
            for(int i = 1; i<= 3; i++) {
                //Fill each deck with cards
                PlayingCard cardBlue({ coordinatePadding + offsetX, SCREEN_HEIGHT - textureHeight - coordinatePadding }, &m_blueCards[i], i, nextCardId());
                PlayingCard cardRed({ coordinatePadding + offsetX, coordinatePadding }, &m_redCards[i], i, nextCardId());
                std::cout << "Initialized card with x:"<< coordinatePadding + offsetX <<" y:"<< SCREEN_HEIGHT - textureHeight - coordinatePadding <<"\n";
                cardBlue.GetTexture()->getRect().x = coordinatePadding + offsetX;
                cardBlue.GetTexture()->getRect().y = SCREEN_HEIGHT - textureHeight - coordinatePadding;
                cardRed.GetTexture()->getRect().x = coordinatePadding + offsetX;
                cardRed.GetTexture()->getRect().y = coordinatePadding;
                PlayingCardsBlue.emplace_back(cardBlue);
                PlayingCardsRed.emplace_back(cardRed);
                offsetX += textureWidth;
            }
        PlayingCard cardBlue({ coordinatePadding + offsetX, SCREEN_HEIGHT - textureHeight - coordinatePadding }, &m_blueCards[4], 4, nextCardId());
        PlayingCardsBlue.emplace_back(cardBlue);

        PlayingCard cardRed({ coordinatePadding + offsetX, coordinatePadding }, &m_redCards[4], 4, nextCardId());
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

    CardTexture blueCardBack(renderer,"Dependencies/textures/blue_back.jpg");
    m_blueCardIllusion = blueCardBack;

    CardTexture redCardBack(renderer, "Dependencies/textures/red_back.jpg");
    m_blueCardIllusion = redCardBack;
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

