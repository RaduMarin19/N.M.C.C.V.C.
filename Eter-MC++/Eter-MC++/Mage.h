#pragma once
#include <random>

#include "MageType.h"

class Mage
{
public:
	Mage();
	void playPower();

private:
	MageType m_mageType;
};

