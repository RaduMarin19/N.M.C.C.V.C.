#pragma once
#include "PlayingCard.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

class Player
{
public:
	Player();
	Player(std::vector<PlayingCard>&& cards);
	Player(Player&&) = default;
	Player& operator=(Player&&) = default;

	void SetGrabbedCard(PlayingCard* grabbedCard);
	void AddCard(const PlayingCard& card);
	void removeCard(const PlayingCard& card);
	std::vector<PlayingCard>& GetCards();
	PlayingCard *GetGrabbedCard() const;
	bool isGrabbingCard() const;
	void SetIsGrabbingCard(bool isGrabbingCard);
	void SetHasPlayedIllusion();
	bool HasPlayedIllusion() const;
	bool& isPlayingIllusion();
	void reset();

private:
	std::vector<PlayingCard> m_cards;
	PlayingCard *m_grabbedCard;
	bool m_isPlayingIllusion;
	bool m_isGrabbingCard;
	bool m_hasPlayedIllusion;
};

