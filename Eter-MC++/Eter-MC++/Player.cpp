#include "Player.h"

Player::Player(std::vector<PlayingCard> cards)
{
	m_cards = cards;
}

void Player::SetGrabbedCard(const std::shared_ptr<PlayingCard>& grabbedCard)
{
	m_grabbedCard = grabbedCard;
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
