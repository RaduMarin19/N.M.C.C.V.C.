#include "PlayingCard.h"
#include <iostream>

PlayingCard::PlayingCard()
{
}

PlayingCard::PlayingCard(const Coordinates& coord, CardTexture* texture, short value, unsigned short id, Color color) :
	Card{ coord,texture,id },
	m_value{ value },
	m_initialValue{ value },
	m_color{ color },
	m_isIllusion{ false }
{
	std::cout << "Called PlayingCard constructor\n";
}

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
	std::cout << "Called PlayingCard copy constructor\n";
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

void to_json(nlohmann::json& j, const PlayingCard& card) {
	j = nlohmann::json{
		{"board_position", {card.GetBoardPosition().GetX(), card.GetBoardPosition().GetY()}},
		{"color", static_cast<int>(card.GetColor())},
		{"value", card.GetValue()},
		{"initial_value", card.GetInitialValue()},
		{"is_illusion", card.IsIllusion()},
		{"is_eter", card.IsEter()}
	};
}

void from_json(const nlohmann::json& j, PlayingCard& card) {
	Coordinates position;

	position.SetX(j.at("board_position").at(0));
	position.SetY(j.at("board_position").at(1));

	Color color = static_cast<Color>(j.at("color").get<int>());
	short value = j.at("value").get<short>();
	short initialValue = j.at("initial_value").get<short>();
	bool isIllusion = j.at("is_illusion").get<bool>();
	bool isEter = j.at("is_eter").get<bool>();

	card.SetBoardPosition(position);
	card.SetValue(value);
	card.SetInitialValue(initialValue);
	card.SetIllusion(isIllusion);
	card.SetEter(isEter);
	
	card.SetColor(color);
}