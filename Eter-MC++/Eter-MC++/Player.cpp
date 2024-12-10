#include "Player.h"

#include <algorithm>
#include <utility>

Player::Player() {
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;
	m_grabbedCard = nullptr;
}

Player::Player(std::vector<PlayingCard>&& cards) : m_cards(std::move(cards)) {
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;
	m_grabbedCard = nullptr;
}

void Player::SetIllusionTexture(std::shared_ptr<CardTexture> texture)
{
	m_cardIllusion = texture;
}

const CardTexture& Player::GetIllusionTexture() const
{
	return *m_cardIllusion;
}

void Player::SetGrabbedCard(PlayingCard* grabbedCard)
{
	m_grabbedCard = grabbedCard;
}

void Player::AddCard(const PlayingCard& card)
{
	m_cards.emplace_back(card);
}

void Player::removeCard(const PlayingCard& card) {
	//Find the given card in the player deck
	auto it = std::remove_if(m_cards.begin(), m_cards.end(),
		[&card](const PlayingCard& c) {
			return c == card; 
		});

	//If it's there, remove it
	if (it != m_cards.end()) {
		m_cards.erase(it, m_cards.end());
	}
	else {
		throw std::runtime_error("Card not found in player's hand.");
	}
}

std::vector<PlayingCard>& Player::GetCards()
{
	return m_cards;
}

PlayingCard *Player::GetGrabbedCard() const
{
		return m_grabbedCard;
}

bool Player::isGrabbingCard() const
{
	return m_isGrabbingCard;
}

void Player::SetIsGrabbingCard(bool isGrabbingCard) {
	this->m_isGrabbingCard = isGrabbingCard;
}

void Player::SetHasPlayedIllusion()
{
	m_hasPlayedIllusion = true;
}

bool Player::HasPlayedIllusion() const
{
	return m_hasPlayedIllusion;
}

bool& Player::isPlayingIllusion()
{
	return m_isPlayingIllusion;
}

void Player::reset()
{
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;
	m_cards.clear();
	m_grabbedCard = nullptr;
}
