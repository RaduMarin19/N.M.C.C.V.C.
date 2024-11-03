#pragma once
#include "GameBoard.h"

class GameModeTournament : public GameBoard
{
public:
	GameModeTournament() {
		setGameMode(GameMode::Tournament);
		setTable(4);
	};
};

