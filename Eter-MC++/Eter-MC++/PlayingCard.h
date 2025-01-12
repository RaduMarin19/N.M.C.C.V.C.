#pragma once
#include "Coordinates.h"
#include "CardTexture.h"
#include "Color.h"
#include "Card.h"

#if defined linux
#include "../Dependencies/JSON/json.hpp"
#else
#include "json.hpp"
#endif


class PlayingCard : public Card
{
public:
	PlayingCard();
	PlayingCard(const Coordinates&, CardTexture*, short, unsigned short, Color);
	PlayingCard(const PlayingCard& other);

	~PlayingCard() = default; // Destructor
	PlayingCard& operator=(const PlayingCard& other) = default;
	PlayingCard(PlayingCard&& other) noexcept = default;
	PlayingCard& operator=(PlayingCard&& other) noexcept = default;

	void SetIllusion(bool IsIllusion);
	void SetEter(bool IsEter);
	bool IsIllusion() const;
	bool IsEter() const;

	Coordinates GetBoardPosition() const;
	Color GetColor() const;
	short GetValue() const;
	short GetInitialValue() const;

	void ReturnToHand();
	void SetBoardPosition(Coordinates position);
	void SetInitialPosition(Coordinates position);
	void SetValue(short value);
	void SetInitialValue(short value);
	void SetColor(Color color);

	bool operator==(const PlayingCard& other) const;

private:
	Coordinates m_boardPosition;
	Color m_color;
	short m_value;
	short m_initialValue;
	bool m_isIllusion;
	bool m_isEter = false;
};

void to_json(nlohmann::json& j, const PlayingCard& card);
void from_json(const nlohmann::json& j, PlayingCard& card);

