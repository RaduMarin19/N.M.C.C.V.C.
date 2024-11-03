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


