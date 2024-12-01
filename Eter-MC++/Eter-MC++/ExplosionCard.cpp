#include "ExplosionCard.h"

template<size_t tableSize>
ExplosionCard<tableSize>::ExplosionCard(uint16_t tableSize)
{
	//here goes the logic to create the explosion mask
	//std::random_device seed;  // Seed
	//std::mt19937 gen(seed()); // Mersenne Twister engine

	//std::uniform_int_distribution<> affectedRange; 
	//std::uniform_int_distribution<> affectedPos; 
	//std::uniform_int_distribution<> explosionType(1, 3);

	//if (tableSize == 3)
	//{
	//	affectedRange = std::uniform_int_distribution<>(2, 4); // range 2-4
	//	affectedPos = std::uniform_int_distribution<>(0, 2); // trebuie adaugat si minX si minY la calcului pozitiei finale
	//}
	//else if (tableSize == 4)
	//{
	//	affectedRange = std::uniform_int_distribution<>(3, 6);
	//	affectedPos = std::uniform_int_distribution<>(0, 3);
	//}

	//m_affectedPosCounter = affectedRange(gen);

	//for (uint16_t i = 0, x, y, generatedType; i < m_affectedPosCounter; ++i)
	//{
	//	x = affectedPos(gen);
	//	y = affectedPos(gen);
	//	generatedType = explosionType(gen);
	//	
	//	// sorry i forgor how to use hashing functions maybe it works maybe it doesn't
	//	
	//	m_affectedPositions.emplace(std::pair<int, int>{x, y}, static_cast<ExplosionType>(generatedType));
	//}
}

template<size_t tableSize>
std::array<std::array<ExplosionType, tableSize>, tableSize>& ExplosionCard<tableSize>::GetExplosionMask() const {
	return m_explosionMask;
}

template<size_t tableSize>
short ExplosionCard<tableSize>::getAffectedPosCounter() const
{
	return m_affectedPosCounter;
}
