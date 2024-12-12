#pragma once
#include "PlayingCard.h"
#include "Card.h"
#include "SpellCard.h"
#include "MageType.h"
#include "ElementalType.h"

#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <variant>
#include <algorithm>
#include <utility>
#include <optional>

class Player
{


public:
	Player();
	Player(std::vector<PlayingCard>&& cards);
	Player(Player&&) = default;
	Player& operator=(Player&&) = default;

	void SetIllusionTexture(std::shared_ptr<CardTexture> texture);
	const CardTexture& GetIllusionTexture() const;

	void SetGrabbedCard(Card* grabbedCard);
	void AddCard(const PlayingCard& card);
	void removeCard(const PlayingCard& card);
	std::vector<PlayingCard>& GetCards();
	Card *GetGrabbedCard() const;
	bool isGrabbingCard() const;
	void SetIsGrabbingCard(bool isGrabbingCard);
	void SetHasPlayedIllusion();
	bool HasPlayedIllusion() const;
	bool& isPlayingIllusion();
	void reset();

private:
	std::vector<PlayingCard> m_cards;
	std::shared_ptr<CardTexture> m_cardIllusion;

	Card *m_grabbedCard;
	bool m_isPlayingIllusion;
	bool m_isGrabbingCard;
	bool m_hasPlayedIllusion;
};

