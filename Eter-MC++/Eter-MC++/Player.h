#pragma once
#include<vector>
#include<memory>
#include<stdexcept>
#include "PlayingCard.h"

class Player
{
public:
	Player(std::vector<PlayingCard> cards);

	void SetGrabbedCard(const std::shared_ptr<PlayingCard>& grabbedCard);
	std::vector<PlayingCard> GetCards() const;
	PlayingCard GetGrabbedCard() const;
	bool isGrabbingCard() const;

private:
	std::vector<PlayingCard> m_cards;
	std::weak_ptr<PlayingCard> m_grabbedCard;
};

