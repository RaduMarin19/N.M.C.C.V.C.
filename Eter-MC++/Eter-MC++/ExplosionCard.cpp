#include "ExplosionCard.h"

#include <iostream>
#include <ostream>


ExplosionCard::ExplosionCard(short tableSize) {
	m_tableSize = tableSize;


	this->m_explosionMask.resize(tableSize);
	for (int i = 0; i < tableSize; i++)
		this->m_explosionMask[i].resize(tableSize);

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

void ExplosionCard::initializeExplosionCard() {
	unsigned short maxIndex = this->m_tableSize * this->m_tableSize;
	unsigned short numberEffects = (rand() % 3) + 2;


	for(int i = 0; i < numberEffects; i++) {
		unsigned short chosenIndex = rand() % maxIndex;
		unsigned short chosenLine = chosenIndex / this->m_tableSize;
		unsigned short chosenColumn = chosenIndex % this->m_tableSize;

		unsigned short chosenEffect = rand() % 100;
		//effect likelyhood - 0-44 return, 44-89 delete, 90-99 hole;
		if(chosenEffect < 45) {
			this->m_explosionMask[chosenLine][chosenColumn] = ExplosionType::RETURN;
		}
		else if (chosenEffect < 90) {
			this->m_explosionMask[chosenLine][chosenColumn] = ExplosionType::DELETE;
		}
		else this->m_explosionMask[chosenLine][chosenColumn] = ExplosionType::HOLE;
	}

}

const std::vector<std::vector<ExplosionType>>& ExplosionCard::GetExplosionMask() const {
	return m_explosionMask;
}

void ExplosionCard::rotateExplosion() {
	//consider a rotation to be 90 degrees clockwise

	//Transposing the matrix
	for(int i=0; i<m_tableSize; i++) {
		for(int j=i+1; j<m_tableSize; j++)
			std::swap(m_explosionMask[i][j], m_explosionMask[j][i]);
	}

	//Reversing each row of the matrix
	for(int i=0; i<m_tableSize; i++) {
		for(int j=0; j<m_tableSize/2; j++) {
			std::swap(m_explosionMask[i][j], m_explosionMask[i][m_tableSize-j-1]);
		}
	}
}


short ExplosionCard::getAffectedPosCounter() const
{
	return m_affectedPosCounter;
}
