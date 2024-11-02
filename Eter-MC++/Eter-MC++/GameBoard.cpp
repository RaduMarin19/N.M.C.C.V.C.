#include "GameBoard.h"
#include <iostream>

GameBoard::GameBoard() : m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 }, m_positions{}, m_possiblePositions{},
m_blueCards{ 0 }, m_redCards{ 0 }, m_explosions{ 0 } {}

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

    /// mai trb adaugat

}
