#include "PlayingCard.h"

PlayingCard::PlayingCard()
{
}

PlayingCard::PlayingCard(const Coordinates& coord, CardTexture* texture, short value, unsigned short id, Color color) :
	Card{ coord,texture,id },
	m_value{ value },
	m_initialValue{ value },
	m_color{ color },
	m_isIllusion{ false } {}

PlayingCard::PlayingCard(const PlayingCard &oth) {
	m_isIllusion = oth.m_isIllusion;
	m_boardPosition = oth.m_boardPosition;
	this->m_position = oth.m_position;
	this->m_value = oth.m_value;
	this->m_texture = oth.m_texture;
	this->m_cardId = oth.m_cardId;
	this->m_color = oth.m_color;
	m_initialPosition = oth.m_initialPosition;
	m_initialValue = oth.m_initialValue;
	m_isEter = oth.m_isEter;
}

void PlayingCard::SetIllusion(bool IsIllusion) {
	m_isIllusion = IsIllusion;
}

bool PlayingCard::IsIllusion() const
{
	return m_isIllusion;
}

void PlayingCard::SetEter(bool isEter)
{
	m_isEter = isEter;
}

bool PlayingCard::IsEter() const
{
	return m_isEter;
}

void PlayingCard::ReturnToHand()
{
	m_boardPosition = m_initialPosition;
}

void PlayingCard::SetBoardPosition(Coordinates position) {
	this->m_boardPosition = position;
}

void PlayingCard::SetInitialPosition(Coordinates position) {
	m_initialPosition = position;
	m_position=position;
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

short PlayingCard::GetInitialValue() const
{
	return m_initialValue;
}

void PlayingCard::SetValue(short value)
{
	m_value = value;
}

void PlayingCard::SetInitialValue(short value)
{
	m_initialValue = value;
}

void PlayingCard::SetColor(Color color)
{
	m_color = color;
}

bool PlayingCard::operator==(const PlayingCard& other) const
{
	return (m_position == other.m_position) && (m_value == other.m_value);
}