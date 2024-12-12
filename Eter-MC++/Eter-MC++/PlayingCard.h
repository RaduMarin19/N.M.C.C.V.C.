#pragma once
#include "Coordinates.h"
#include "CardTexture.h"
#include "Color.h"
#include "Card.h"

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

	void SetIllussion(bool IsIllusion);
	bool IsIllusion() const;

	Coordinates GetBoardPosition() const;
	Color GetColor() const;
	short GetValue() const;

	void SetBoardPosition(Coordinates position);
	void SetInitialPosition(Coordinates position);
	void SetValue(short value);

	bool operator==(const PlayingCard& other) const;

private:
	Coordinates m_boardPosition;
	Color m_color;
	short m_value;
	bool m_isIllusion;
};

