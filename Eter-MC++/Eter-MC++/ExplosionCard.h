#include "ExplosionType.h"

#include <random>
#include <unordered_map>

template<size_t tableSize=3>
class ExplosionCard
{
public:
	struct pair_hash {
		size_t operator()(const std::pair<int, int>& key) const {
			return std::hash<int>()(key.first) ^ (std::hash<int>()(key.second) << 1);
		}
	};
	
	ExplosionCard(uint16_t tableSize);

	std::array<std::array<ExplosionType, tableSize>, tableSize>& GetExplosionMask() const;

	short getAffectedPosCounter() const;

private:
	short m_affectedPosCounter;
	std::array<std::array<ExplosionType, tableSize>, tableSize> m_explosionMask;
};