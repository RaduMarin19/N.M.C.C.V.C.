#pragma once
#include "Coordinates.h"
#include "CardTexture.h"
#include "Color.h"

class PlayingCard
{
public:
	PlayingCard(Coordinates position, short value);
	PlayingCard(const Coordinates&, CardTexture*, short, unsigned short, Color);
	PlayingCard(const PlayingCard&);

	Coordinates GetCoordinates() const;
	Coordinates GetBoardPosition() const;
	Color GetColor() const;
	short GetValue() const;
	unsigned short GetId() const;

	CardTexture* GetTexture() const;

	void SetBoardPosition(Coordinates position);
	void SetCoordinates(const Coordinates& position);
	void setValue(short value);

	bool operator==(const PlayingCard& other) const;

private:
	Coordinates m_boardPosition;
	Coordinates m_position;
	Color m_color;
	short m_value;
	CardTexture* m_texture;
	unsigned short m_cardId;
};

