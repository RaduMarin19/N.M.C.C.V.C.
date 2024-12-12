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
	SpellCard(const Coordinates& position, CardTexture* texture,ElementalType spell, unsigned short id);
	ElementalType GetSpell() const;
	void SetSpell(ElementalType spell, CardTexture* texture);

private:
	ElementalType m_spell;
};

