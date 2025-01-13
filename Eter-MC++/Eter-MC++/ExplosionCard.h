#include "ExplosionType.h"
#include "Random.h"
#include "Coordinates.h"

#include <vector>

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

	void MakeExplosionFromVector(std::vector<std::pair<Coordinates, ExplosionType>>);

	ExplosionCard(const ExplosionCard&) = delete;                 
	ExplosionCard& operator=(const ExplosionCard&) = delete;       
	ExplosionCard(ExplosionCard&&) = default;                     
	ExplosionCard& operator=(ExplosionCard&&) = default;

private:
	short m_tableSize;
	std::vector<std::vector<ExplosionType>> m_explosionMask;
};
