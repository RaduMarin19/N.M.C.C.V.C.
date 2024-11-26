#pragma once
#include "Coordinates.h"
#include "CardTexture.h"
#include "Color.h"
#include "Card.h"

class PlayingCard : public Card
{
public:
	PlayingCard(Coordinates position, short value);
	PlayingCard(const Coordinates&, CardTexture*, short, unsigned short, Color);
	PlayingCard(const PlayingCard&);

	void SetIllussion(bool isIllusion);
	bool isIllusion() const;

	Coordinates GetBoardPosition() const;
	Color GetColor() const;
	short GetValue() const;

	void SetBoardPosition(Coordinates position);
	void SetInitialPosition(Coordinates position);
	void setValue(short value);

	bool operator==(const PlayingCard& other) const;

private:
	Coordinates m_boardPosition;
	Color m_color;
	short m_value;
	bool m_isIllusion;
};

