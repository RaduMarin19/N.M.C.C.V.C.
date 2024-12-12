#include "ExplosionType.h"
#include "Random.h"

#include <unordered_map>

class ExplosionCard
{
public:
	struct pair_hash {
		size_t operator()(const std::pair<int, int>& key) const {
			return std::hash<int>()(key.first) ^ (std::hash<int>()(key.second) << 1);
		}
	};

	ExplosionCard(short tableSize);

	void InitializeExplosionCard();

	void RotateExplosion();

	const std::vector<std::vector<ExplosionType>>& GetExplosionMask() const;

	short GetAffectedPosCounter() const;

	ExplosionCard(const ExplosionCard&) = delete;                 
	ExplosionCard& operator=(const ExplosionCard&) = delete;       
	ExplosionCard(ExplosionCard&&) = default;                     
	ExplosionCard& operator=(ExplosionCard&&) = default;

private:
	short m_tableSize;
	short m_affectedPosCounter;
	std::vector<std::vector<ExplosionType>> m_explosionMask;
};