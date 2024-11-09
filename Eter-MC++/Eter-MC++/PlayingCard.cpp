#include "PlayingCard.h"

PlayingCard::PlayingCard(Coordinates position, short value) : m_position{position}, m_value{value}{}

PlayingCard::PlayingCard(const Coordinates& coord, CardTexture* texture, short value, unsigned short id)
{
	this->m_cardId = id;
	this->m_position = coord;
	this->m_value = value;
	this->m_texture = texture;
}

PlayingCard::PlayingCard(const PlayingCard &oth) {
	m_position = oth.m_position;
	m_value = oth.m_value;
	m_texture = oth.m_texture;
	m_cardId = oth.m_cardId;
}

Coordinates PlayingCard::GetCoordinates() const
{
	return m_position;
}

void PlayingCard::SetBoardPosition(Coordinates position) {
	m_boardPosition = position;
}

Coordinates PlayingCard::GetBoardPosition() const {
	return m_boardPosition;
}

short PlayingCard::GetValue() const
{
	return m_value;
}

unsigned short PlayingCard::GetId() const {
	return m_cardId;
}

CardTexture* PlayingCard::GetTexture() const
{
	return m_texture;
}

void PlayingCard::SetCoordinates(const Coordinates& position)
{
	m_position = position;
}

void PlayingCard::setValue(short value)
{
	m_value = value;
}

bool PlayingCard::operator==(const PlayingCard& other) const
{
	return (m_position == other.m_position) && (m_value == other.m_value);
}
