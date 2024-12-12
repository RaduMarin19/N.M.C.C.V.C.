#pragma once
#include "Card.h"
#include "Coordinates.h"
#include "CardTexture.h"
#include "MageType.h"
#include "ElementalType.h"

#include <variant>

class SpellCard : public Card
{
public:
	using SpellType = std::variant<MageType, ElementalType>;  //no value for training - other types for specific gamemodes

public:
	SpellCard(const Coordinates& position, CardTexture* texture,SpellType spell, unsigned short id);
	SpellType GetSpell() const;
	void SetSpell(SpellType spell, CardTexture* texture);

private:
	SpellType m_spell;
};

