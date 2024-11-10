#include "PlayingCard.h"

PlayingCard::PlayingCard(Coordinates position, short value) : m_position{position}, m_value{value}{}

PlayingCard::PlayingCard(const Coordinates& coord, CardTexture* texture, short value, unsigned short id, Color color)
{
	m_isIllusion = false;
	this->m_color = color;
	this->m_cardId = id;
	this->m_position = coord;
	m_initialPosition = coord;
	this->m_value = value;
	this->m_texture = texture;
}

PlayingCard::PlayingCard(const PlayingCard &oth) {
	m_isIllusion = oth.m_isIllusion;
	this->m_position = oth.m_position;
	this->m_value = oth.m_value;
	this->m_texture = oth.m_texture;
	this->m_cardId = oth.m_cardId;
	this->m_color = oth.m_color;
	m_initialPosition = oth.m_initialPosition;
}

void PlayingCard::SetIllussion(bool isIllusion) {
	m_isIllusion = isIllusion;
}

bool PlayingCard::isIllusion() const
{
	return m_isIllusion;
}

Coordinates PlayingCard::GetCoordinates() const
{
	return m_position;
}

void PlayingCard::SetBoardPosition(Coordinates position) {
	this->m_boardPosition = position;
}

Coordinates PlayingCard::GetBoardPosition() const {
	return m_boardPosition;
}

Coordinates PlayingCard::GetInitialPosition() const {
	return m_initialPosition;
}

Color PlayingCard::GetColor() const
{
	return m_color;
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
