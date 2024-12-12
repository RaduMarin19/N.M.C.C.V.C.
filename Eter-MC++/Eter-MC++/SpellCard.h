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
	
	void SetSpell(ElementalType spell, CardTexture* texture);
	void SetUsed(bool used);

	bool IsUsed() const;
	ElementalType GetSpell() const;

private:
	ElementalType m_spell;
	bool m_used = false;
};

