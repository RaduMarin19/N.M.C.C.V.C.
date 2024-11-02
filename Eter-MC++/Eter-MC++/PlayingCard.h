#pragma once
#include "Coordinates.h"

class PlayingCard
{
public:
	PlayingCard(Coordinates position, short value);

	Coordinates GetCoordinates() const;
	short GetValue() const;

	void SetCoordinates(const Coordinates& position);
	void setValue(short value);

private:
	Coordinates m_position;
	short m_value;

	//to do: add textures
};

