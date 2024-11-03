#pragma once
#include "GameBoard.h"

class GameModeElemental : public GameBoard
{
	GameModeElemental() {
		setGameMode(GameMode::Elemental);
		setTable(4);
	}
private:
	std::vector<CardTexture> m_ElementalCards;
};

