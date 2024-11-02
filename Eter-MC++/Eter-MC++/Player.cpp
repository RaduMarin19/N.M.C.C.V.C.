#include "Player.h"

Player::Player()
{
}

Player::Player(std::vector<PlayingCard> cards)
{
	m_cards = cards;
}

void Player::SetGrabbedCard(const std::shared_ptr<PlayingCard>& grabbedCard)
{
	m_grabbedCard = grabbedCard;
}

void Player::AddCard(const PlayingCard& card)
{
	m_cards.emplace_back(card);
}

void Player::removeCard(const PlayingCard& card) {
	auto it = std::remove_if(m_cards.begin(), m_cards.end(),
		[&card](const PlayingCard& c) {
			return c == card; 
		});

	if (it != m_cards.end()) {
		m_cards.erase(it, m_cards.end());
	}
	else {
		throw std::runtime_error("Card not found in player's hand.");
	}
}

std::vector<PlayingCard> Player::GetCards() const
{
	return m_cards;
}

PlayingCard Player::GetGrabbedCard() const
{
	if (auto grabbedCard = m_grabbedCard.lock()) {
		return *grabbedCard;  
	}
	else {
		throw std::runtime_error("No card is currently grabbed.");
	}
}

bool Player::isGrabbingCard() const
{
	return !m_grabbedCard.expired();
}
