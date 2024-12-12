#include "SpellCard.h"

SpellCard::SpellCard(const Coordinates& position, CardTexture* texture,SpellType spell, unsigned short id) :Card{position, texture, id},m_spell{spell}
{
}

SpellCard::SpellType SpellCard::GetSpell() const
{
	return m_spell;
}

void SpellCard::SetSpell(SpellType spell, CardTexture* texture)
{
	m_spell = spell;
	m_texture = texture;
}
