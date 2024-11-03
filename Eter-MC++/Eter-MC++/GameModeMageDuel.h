#pragma once
#include "GameBoard.h"

class GameModeMageDuel : public GameBoard
{
public:
	GameModeMageDuel() {
		setGameMode(GameMode::MageDuel);
		setTable(4);
	};
private:
	std::vector<CardTexture> m_MageCards;
};

