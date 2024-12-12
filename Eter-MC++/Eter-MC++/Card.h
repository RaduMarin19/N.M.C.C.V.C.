#pragma once
#include "Coordinates.h"
#include "CardTexture.h"
#include "Color.h"

class Card
{
public:
	Card(const Coordinates& position);
	Card();
	Card(const Card& other);
	Card(const Coordinates& position, CardTexture* texture,unsigned short id);

	virtual ~Card() = default; //for polimorphism

	Coordinates GetInitialPosition() const;
	Coordinates GetCoordinates() const;
	unsigned short GetId() const;
	CardTexture* GetTexture() const;

	void SetCoordinates(const Coordinates& position);

	bool operator==(const Card& other) const;
protected:
	Coordinates m_position;
	Coordinates m_initialPosition;
	CardTexture* m_texture;
	unsigned short m_cardId;
};

