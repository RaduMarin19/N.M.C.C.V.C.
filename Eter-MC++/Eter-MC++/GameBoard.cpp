#include "GameBoard.h"
#include <iostream>

GameBoard::GameBoard() : m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 }, m_positions{}, m_possiblePositions{},
m_blueCards{ 0 }, m_redCards{ 0 }, m_explosions{ 0 } {}

void GameBoard::testPossiblePosition(short x, short y)
{
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
    if (!this->m_positions.contains({ x, y }))
        this->m_possiblePositions.emplace(x, y);
}

void GameBoard::pushNewCard(PlayingCard otherCard)
{
	Coordinates newCardCoords = otherCard.GetCoordinates();

    if (newCardCoords.GetX() < m_minX) this->m_minX = newCardCoords.GetX();  //updating the minimum and maximum
    if (newCardCoords.GetX() > m_maxX) this->m_maxX = newCardCoords.GetX();  //coordinates values for the gameBoard

    if (newCardCoords.GetY() < m_minY) this->m_minY = newCardCoords.GetY();
    if (newCardCoords.GetY() > m_maxY) this->m_maxY = newCardCoords.GetY();

    if (!m_possiblePositions.contains(newCardCoords)) {
        std::cout << "Not playing card because not a possible position." << newCardCoords.GetX() << " " << newCardCoords.GetY() << "\n";
        return;
    }

    if (!m_positions.contains(newCardCoords)) {
        std::stack<PlayingCard> cards;
        cards.emplace(otherCard);
        m_positions.emplace(newCardCoords, cards);
    }
    else {
        auto it = m_positions.find(newCardCoords);
        it->second.emplace(otherCard);
    }

    {

        //vecini stanga dreapta
        this->testPossiblePosition(newCardCoords.GetX() - 1, newCardCoords.GetY());
        this->testPossiblePosition(newCardCoords.GetX() + 1, newCardCoords.GetY());

        //vecini sus jos
        this->testPossiblePosition(newCardCoords.GetX(), newCardCoords.GetY() - 1);
        this->testPossiblePosition(newCardCoords.GetX(), newCardCoords.GetY() + 1);

        //vecini diagonali stanga
        this->testPossiblePosition(newCardCoords.GetX() - 1, newCardCoords.GetY() - 1);
        this->testPossiblePosition(newCardCoords.GetX() - 1, newCardCoords.GetY() + 1);

        //vecini diagonali dreapta
        this->testPossiblePosition(newCardCoords.GetX() + 1, newCardCoords.GetY() - 1);
        this->testPossiblePosition(newCardCoords.GetX() + 1, newCardCoords.GetY() + 1);
    }

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
        std::vector<PlayingCard> PlayingCardsBlue;
        std::vector<PlayingCard> PlayingCardsRed;

        int offsetX = 0;

        for(int i = 0; i < 2; i++)
            for(int i = 1; i<= 3; i++) {
                PlayingCard cardBlue({ coordinatePadding + offsetX, SCREEN_HEIGHT - textureHeight - coordinatePadding }, &m_blueCards[i], i, nextCardId());
                PlayingCard cardRed({ coordinatePadding + offsetX, coordinatePadding }, &m_redCards[i], i, nextCardId());
                PlayingCardsBlue.emplace_back(cardBlue);
                PlayingCardsRed.emplace_back(cardRed);
                offsetX += textureWidth / 2;
            }
        PlayingCard cardBlue({ coordinatePadding + offsetX, SCREEN_HEIGHT - textureHeight - coordinatePadding }, &m_blueCards[4], 4, nextCardId());
        PlayingCardsBlue.emplace_back(cardBlue);

        PlayingCard cardRed({ coordinatePadding + offsetX, coordinatePadding }, &m_redCards[4], 4, nextCardId());
        PlayingCardsRed.emplace_back(cardRed);

        Player playerBlue(PlayingCardsBlue);
        this->m_playerBlue = playerBlue;

        Player playerRed(PlayingCardsRed);
        this->m_playerRed = playerRed;
    } else if (mode == GameMode::Elemental) {

    } else if (mode == GameMode::MageDuel) {

    } else if (mode == GameMode::Tournament) {

    }
}

bool GameBoard::getCardAtPosition(const Coordinates &coordinates, PlayingCard &card) const {
    auto PlayingCard = this->m_positions.find(coordinates);
    if(PlayingCard != this->m_positions.end()) {
        card = PlayingCard->second.top();
        return true;
    } return false;

}

Player *GameBoard::getPlayerRed() {
    return &m_playerRed;
}

Player *GameBoard::getPlayerBlue() {
    return &m_playerBlue;
}

GameBoard::GameBoard(SDL_Renderer* renderer)
{
    this->m_possiblePositions.emplace(0, 0);

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
#endif

}

unsigned short GameBoard::nextCardId() {
    return ++GameBoard::m_cardId;
}


