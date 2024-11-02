#pragma once
#include "Coordinates.h"
#include "PlayingCard.h"
#include "CardTexture.h"
#include "Game.h"

#include <unordered_map>
#include <unordered_set>
#include <stack>

class GameBoard : public Game
{
public:
	GameBoard();
	void pushNewCard(PlayingCard other);
	// still need to add some functions
private:
	short m_minX, m_maxX, m_minY, m_maxY;
	static short table; // it varies between 3 and 4 depending on game mode

	std::unordered_map<Coordinates, std::stack<PlayingCard>, Coordinates> m_positions;
	std::unordered_set<Coordinates, Coordinates> m_possiblePositions;

	std::vector<CardTexture> m_blueCards;
	std::vector<CardTexture> m_redCards;
	std::vector<CardTexture> m_explosions;
};

