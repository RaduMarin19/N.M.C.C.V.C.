#pragma once

#include "MageType.h"
#include "GameBoard.h"

#include <random>
#include <iostream>

class Mage
{
public:
	Mage();
	void playPower(GameBoard& board);

private:
	MageType m_mageType;
};

