#pragma once
#include "Coordinates.h"
#include "CardTexture.h"

class PlayingCard
{
public:
	PlayingCard(Coordinates position, short value);
	PlayingCard(const Coordinates&, CardTexture*, short, unsigned short);
	PlayingCard(const PlayingCard&);

	Coordinates GetCoordinates() const;
	short GetValue() const;

	CardTexture* GetTexture() const;

	void SetCoordinates(const Coordinates& position);
	void setValue(short value);

	bool operator==(const PlayingCard& other) const;

private:
	Coordinates m_position;
	short m_value;
	CardTexture* m_texture;
	unsigned short m_cardId;

	//to do: add textures
};

