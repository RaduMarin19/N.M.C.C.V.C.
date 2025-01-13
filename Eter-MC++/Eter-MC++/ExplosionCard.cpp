#include "ExplosionCard.h"

#include <iostream>
#include <ostream>

#include "Coordinates.h"

ExplosionCard::ExplosionCard(short tableSize) {
	m_tableSize = tableSize;

	this->m_explosionMask.resize(tableSize);
	for (int i = 0; i < tableSize; i++)
		this->m_explosionMask[i].resize(tableSize);
}

void ExplosionCard::InitializeExplosionCard() {
	unsigned short maxIndex = this->m_tableSize * this->m_tableSize - 1;
	unsigned short numberEffects = Random::Get(2, 4);

	for (int i = 0; i < m_explosionMask.size(); ++i) {
		std::fill(m_explosionMask[i].begin(), m_explosionMask[i].end(), ExplosionType::NONE);
	}

	for (int i = 0; i < numberEffects; i++) {
		unsigned short chosenIndex = Random::Get(0, maxIndex);
		unsigned short chosenLine = chosenIndex / this->m_tableSize;
		unsigned short chosenColumn = chosenIndex % this->m_tableSize;

		unsigned short chosenEffect = Random::Get(0, 99);
		//effect likelyhood - 0-44 return, 44-89 delete, 90-99 hole;
		if (chosenEffect < 45) {
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

void ExplosionCard::RotateExplosion() {
	//consider a rotation to be 90 degrees clockwise

	//Transposing the matrix
	for (int i = 0; i < m_tableSize; i++) {
		for (int j = i + 1; j < m_tableSize; j++)
			std::swap(m_explosionMask[i][j], m_explosionMask[j][i]);
	}

	//Reversing each row of the matrix
	for (int i = 0; i < m_tableSize; i++) {
		for (int j = 0; j < m_tableSize / 2; j++) {
			std::swap(m_explosionMask[i][j], m_explosionMask[i][m_tableSize - j - 1]);
		}
	}
}

void ExplosionCard::MakeExplosionFromVector(std::vector<std::pair<Coordinates, ExplosionType>> vec) {
	for (int i = 0; i < m_explosionMask.size(); ++i) {
		std::fill(m_explosionMask[i].begin(), m_explosionMask[i].end(), ExplosionType::NONE);
	}
	for (const auto& [pos, eff] : vec) {
		m_explosionMask[pos.GetX()][pos.GetY()] = eff;
	}
}

void ExplosionCard::SaveExplosionToJson(nlohmann::json& json)
{
	for (int i = 0; i < m_explosionMask.size(); ++i) {
		for (int j = 0; j < m_explosionMask[i].size(); ++j) {
			json.push_back({
				{"position", { {"i", i}, {"j", j} }},
				{"explosionType", m_explosionMask[i][j]}
				});
		}
	}
}

void ExplosionCard::LoadExplosionFromJson(const nlohmann::json& json) {

	for (int i = 0; i < m_explosionMask.size(); ++i) {
		std::fill(m_explosionMask[i].begin(), m_explosionMask[i].end(), ExplosionType::NONE);
	}

	for (const auto& element : json) {
		int i = element["position"]["i"];
		int j = element["position"]["j"];
		ExplosionType explosionType = static_cast<ExplosionType>(element["explosionType"]);

		if (i >= 0 && i < m_explosionMask.size() && j >= 0 && j < m_explosionMask[i].size()) {
			m_explosionMask[i][j] = explosionType;
		}
	}
}

