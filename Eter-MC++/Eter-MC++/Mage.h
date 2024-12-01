#pragma once
#include <random>

#include "MageType.h"
#include "GameBoard.h"

class Mage
{
public:
	Mage();
	void playPower(GameBoard& board);

private:
	MageType m_mageType;
};

