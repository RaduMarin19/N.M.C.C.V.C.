#include "ExplosionCard.h"

ExplosionCard::ExplosionCard()
{
	std::random_device seed;  // Seed
	std::mt19937 gen(seed()); // Mersenne Twister engine

	/// Asta e pentru jocul cu tabla de 3x3
	/// Trebuie sa adaug si pt 4x4

	std::uniform_int_distribution<> affectedRange(2, 4); // range 0-99
	std::uniform_int_distribution<> affectedPos(0, 2); // trebuie adaugat si minX si minY la calcului pozitiei finale

	m_affectedPosCounter = affectedRange(gen);

	for (uint16_t i = 0, x, y; i < m_affectedPosCounter; ++i)
	{
		x = affectedPos(gen);
		y = affectedPos(gen);
		m_affectedPositions.push_back({x, y});
	}
}

void ExplosionCard::setExplosionType(const ExplosionType& type)
{
	m_explosionType = type;
}

ExplosionType ExplosionCard::getExplosionType() const
{
	return m_explosionType;
}

std::vector<std::pair<int, int>>& ExplosionCard::getAffectedPos()
{
	return m_affectedPositions;
}

short ExplosionCard::getAffectedPosCounter() const
{
	return m_affectedPosCounter;
}
