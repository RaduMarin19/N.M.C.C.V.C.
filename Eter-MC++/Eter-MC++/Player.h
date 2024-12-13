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
	Player(std::vector<PlayingCard>& cards);
	Player(Player&&) = default;
	Player& operator=(Player&&) = default;
	Player(const Player&) = delete; 
	Player& operator=(const Player&) = delete;

	~Player() = default;

	void SetIllusionTexture(std::shared_ptr<CardTexture> texture);
	const CardTexture& GetIllusionTexture() const;

	void SetGrabbedCard(Card* grabbedCard);
	void AddCard(const PlayingCard& card);
	void AddRemovedCard(PlayingCard& card);    //for cards that are removed from the game
	void RemoveCardFromHand(const PlayingCard& card);

	std::vector<PlayingCard>& GetRemovedCards();
	std::vector<PlayingCard>& GetCards();

	Card *GetGrabbedCard() const;
	bool IsGrabbingCard() const;

	void SetIsGrabbingCard(bool isGrabbingCard);
	void SetHasPlayedIllusion();
	void SetIsPlayingAshes(bool isPlayingAshes);

	bool HasPlayedIllusion() const;
	bool& IsPlayingIllusion();
	bool IsPlayingAshes() const;

	void Reset();

private:
	std::vector<PlayingCard> m_cards;
	std::vector<PlayingCard> m_removedCards;
	std::shared_ptr<CardTexture> m_cardIllusion;

	Card *m_grabbedCard;
	bool m_isPlayingIllusion;
	bool m_isGrabbingCard;
	bool m_hasPlayedIllusion;
	bool m_isPlayingAshes;

	void RemoveCardFromDeck(std::vector<PlayingCard>& cards, const PlayingCard& card);
};

