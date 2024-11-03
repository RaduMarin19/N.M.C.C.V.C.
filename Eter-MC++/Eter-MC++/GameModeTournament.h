#pragma once
#include "GameBoard.h"

class GameModeTournament : public GameBoard
{
public:
	GameModeTournament() {
		m_gameMode = GameBoard::GameMode::Tournament;
	};
};

