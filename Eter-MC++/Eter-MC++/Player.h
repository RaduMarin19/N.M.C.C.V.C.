#pragma once
#include<vector>
#include<memory>
#include<stdexcept>
#include "PlayingCard.h"

class Player
{
public:
	Player();
	Player(std::vector<PlayingCard> cards);

	void SetGrabbedCard(PlayingCard* grabbedCard);
	void AddCard(const PlayingCard& card);
	void removeCard(const PlayingCard& card);
	std::vector<PlayingCard>& GetCards();
	PlayingCard *GetGrabbedCard() const;
	bool isGrabbingCard() const;
	void SetIsGrabbingCard(bool isGrabbingCard);

private:
	std::vector<PlayingCard> m_cards;
	PlayingCard *m_grabbedCard;
	bool m_isGrabbingCard;
};

