#include "PlayingCard.h"

PlayingCard::PlayingCard(Coordinates position, short value) : m_position{position}, m_value{value}{}

Coordinates PlayingCard::GetCoordinates() const
{
	return m_position;
}

short PlayingCard::GetValue() const
{
	return m_value;
}

void PlayingCard::SetCoordinates(const Coordinates& position)
{
	m_position = position;
}

void PlayingCard::setValue(short value)
{
	m_value = value;
}
