#pragma once
#include "PlayingCard.h"
#include "Card.h"
#include "Color.h"
#include "SpellCard.h"
#include "ElementalType.h"
#include "WizardType.h"
#include "WizardCard.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <variant>
#include <algorithm>
#include <utility>
#include <optional>
#include <concepts>
#include <iterator>
#include <stack>

template <typename Container, typename T>
concept CardContainer =
std::ranges::input_range<Container> &&                        // Container must be an input range
std::equality_comparable_with<typename Container::value_type, T>&& // Elements must be comparable with T
	requires(Container c, typename Container::iterator it) {
		{ c.erase(it) };                                          // Container must support erase()
};

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
	void SetWizard(WizardType wizard, CardTexture* texture, Coordinates position, unsigned short cardId);
	const std::unique_ptr<WizardCard>& GetWizardCard() const;
	void RemoveWizard();

	template <typename Container>
	requires CardContainer<Container, PlayingCard>
	void AddRemovedCard(Container& container, PlayingCard& card);    //for cards that are removed from the game

	void RemoveCardFromHand(const PlayingCard& card);

	const std::vector<PlayingCard>& GetRemovedCards() const;
	const std::vector<PlayingCard>& GetCards() const;
	std::vector<PlayingCard>& GetRemovedCards();
	std::vector<PlayingCard>& GetCards();

	std::stack<PlayingCard>& GetOrderOfPlayedCards();
	void AddCardToOrderStack(PlayingCard card);

	Card *GetGrabbedCard() const;
	bool IsGrabbingCard() const;

	void SetIsGrabbingCard(bool isGrabbingCard);
	void SetHasPlayedIllusion();
	void SetIsPlayingAshes(bool isPlayingAshes);


	bool HasPlayedIllusion() const;
	bool& IsPlayingIllusion();
	bool IsPlayingAshes() const;

	void LoadRemovedCard(PlayingCard& card);
	void SetColor(Color color);
	 
	const Color GetColor() const;

	void ClearCards();
	void ClearRemovedCards();
	void Reset();

private:
	std::vector<PlayingCard> m_cards;
	std::vector<PlayingCard> m_removedCards;
	std::shared_ptr<CardTexture> m_cardIllusion;
	std::unique_ptr<WizardCard> m_wizard;

	std::stack<PlayingCard> m_OrderOfPlayedCards;

	Color m_color;

	Card *m_grabbedCard;
	bool m_isPlayingIllusion;
	bool m_isGrabbingCard;
	bool m_hasPlayedIllusion;
	bool m_isPlayingAshes;

	void RemoveCardFromDeck(std::vector<PlayingCard>& cards, const PlayingCard& card);
};

template <typename Container>
requires CardContainer<Container, PlayingCard>
void Player::AddRemovedCard(Container& container, PlayingCard& card) {
	card.ReturnToHand();

	auto it = std::find_if(container.begin(), container.end(),
		[&card](const PlayingCard& c) {
			return c == card;
		});

	// If it's there, move it to removed cards and erase it from the hand
	if (it != container.end()) {
		m_removedCards.emplace_back(std::move(*it)); // Move the card to removedCards
		container.erase(it);                        // Remove the card from the player's hand
	}
	else {
		throw std::runtime_error("Card not found in player's hand.");
	}
}

