#pragma once
#include "Card.h"
#include "Coordinates.h"
#include "CardTexture.h"
#include "ElementalType.h"

#include <variant>

class SpellCard : public Card
{
public:
	SpellCard(const Coordinates& position, CardTexture* texture,ElementalType spell, unsigned short id);
	
	~SpellCard() = default; 
	SpellCard(const SpellCard& other) = default; 
	SpellCard& operator=(const SpellCard& other) = default; 
	SpellCard(SpellCard&& other) noexcept = default; 
	SpellCard& operator=(SpellCard&& other) noexcept = default; 

	void SetSpell(ElementalType spell, CardTexture* texture);
	void SetUsed(bool used);

	bool IsUsed() const;
	ElementalType GetSpell() const;

private:
	ElementalType m_spell;
	bool m_used;
};

