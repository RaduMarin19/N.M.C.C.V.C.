#pragma once
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include "PlayingCard.h"
#include "Mage.h"
#include "MageType.h"

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
	std::unordered_map<MageType, std::vector<std::vector<uint8_t>>> m_magesList; //every mage has his own ability
};

