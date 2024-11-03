#pragma once
#include "GameBoard.h"

class GameModeTraining : public GameBoard
{
public:
	GameModeTraining() {
		setGameMode(GameMode::Training);
		setTable(3);
	}
private:

};

