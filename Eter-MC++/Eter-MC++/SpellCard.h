#pragma once
#include "Card.h"
#include "Coordinates.h"
#include "CardTexture.h"

class SpellCard : public Card
{
public:
	SpellCard(const Coordinates& position, CardTexture* texture, unsigned short id);

private:
};

