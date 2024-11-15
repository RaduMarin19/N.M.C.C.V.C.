#include "ExplosionType.h"

#include <random>
#include <unordered_map>

class ExplosionCard
{
public:
	struct pair_hash {
		size_t operator()(const std::pair<int, int>& key) const {
			return std::hash<int>()(key.first) ^ (std::hash<int>()(key.second) << 1);
		}
	};
	
	ExplosionCard(uint16_t tableSize);

	std::unordered_map<std::pair<int, int>, ExplosionType, pair_hash>& getAffectedPos();

	short getAffectedPosCounter() const;

private:
	short m_affectedPosCounter;
	std::unordered_map<std::pair<int, int>, ExplosionType, pair_hash> m_affectedPositions; /// hashing function doesn't work
};