#include "SpellCard.h"

SpellCard::SpellCard(const Coordinates& position, CardTexture* texture, ElementalType spell, unsigned short id) :
	Card{position, texture, id}, m_spell{spell}
{
}

ElementalType SpellCard::GetSpell() const
{
	return m_spell;
}

void SpellCard::SetSpell(ElementalType spell, CardTexture* texture)
{
	m_spell = spell;
	m_texture = texture;
}

void SpellCard::SetUsed(bool used)
{
	m_used = used;
}

bool SpellCard::IsUsed() const
{
	return m_used;
}
