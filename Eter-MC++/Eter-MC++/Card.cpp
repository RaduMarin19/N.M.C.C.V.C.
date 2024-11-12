#include "Card.h"

Card::Card(const Coordinates& position) : m_position{ position } {}

Card::Card(){}

Card::Card(const Card& other) :
	m_cardId{ other.m_cardId },
	m_position{ other.m_position },
	m_initialPosition{ other.m_position },
	m_texture{ other.m_texture }
	{}

Card::Card(const Coordinates& position, CardTexture* texture, unsigned short id) :
	m_cardId{ id },
	m_position{ position },
	m_initialPosition{ position },
	m_texture{ texture }
	{}

Coordinates Card::GetInitialPosition() const {
	return m_initialPosition;
}

bool Card::operator==(const Card& other) const {
	return m_cardId == other.m_cardId;
}

Coordinates Card::GetCoordinates() const {
	return m_position;
}

unsigned short Card::GetId() const {
	return m_cardId;
}

CardTexture* Card::GetTexture() const
{
	return m_texture;
}

void Card::SetCoordinates(const Coordinates& position)
{
	m_position = position;
}
