#include "ExplosionType.h"
#include <random>
#include <unordered_map>

class ExplosionCard
{
public:
	ExplosionCard();

	void setExplosionType(const ExplosionType& type);
	ExplosionType getExplosionType() const;

	std::vector<std::pair<int, int>>& getAffectedPos();
	short getAffectedPosCounter() const;

private:
	ExplosionType m_explosionType;
	short m_affectedPosCounter;
	std::vector<std::pair<int, int>> m_affectedPositions;
};