#include "PlayingCard.h"

PlayingCard::PlayingCard(Coordinates position, short value) : 
	Card{ position }, 
	m_value{ value },
	m_isIllusion{false}
	{}

PlayingCard::PlayingCard(const Coordinates& coord, CardTexture* texture, short value, unsigned short id, Color color) :
	Card{coord,texture,id},
	m_value{value},
	m_color{color},
    m_isIllusion{ false }
	{}

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

void PlayingCard::SetBoardPosition(Coordinates position) {
	this->m_boardPosition = position;
}

void PlayingCard::SetInitialPosition(Coordinates position) {
	m_initialPosition = position;
}

Coordinates PlayingCard::GetBoardPosition() const {
	return m_boardPosition;
}

Color PlayingCard::GetColor() const
{
	return m_color;
}

short PlayingCard::GetValue() const
{
	return m_value;
}

void PlayingCard::setValue(short value)
{
	m_value = value;
}

bool PlayingCard::operator==(const PlayingCard& other) const
{
	return (m_position == other.m_position) && (m_value == other.m_value);
}
