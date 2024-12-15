#include "Player.h"

Player::Player() {
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;

	m_grabbedCard = nullptr;
}

Player::Player(std::vector<PlayingCard>& cards) : m_cards(std::move(cards)) {
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;
	m_isPlayingAshes = false;

	m_color = m_cards.back().GetColor();

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

void Player::SetGrabbedCard(Card* grabbedCard)
{
	m_grabbedCard = grabbedCard;
}

void Player::AddCard(const PlayingCard& card)
{
	m_cards.emplace_back(card);
}

void Player::RemoveCardFromDeck(std::vector<PlayingCard>& cards,const PlayingCard& card) {
	auto it = std::remove_if(cards.begin(), cards.end(),
		[&card](const PlayingCard& c) {
			return c == card;
		});

	//If it's there, remove it
	if (it != cards.end()) {
		cards.erase(it, cards.end());
	}
	else {
		throw std::runtime_error("Card not found in player's hand.");
	}
}

void Player::RemoveCardFromHand(const PlayingCard& card) {
	//Find the given card in the player deck
	if (m_isPlayingAshes) {
		RemoveCardFromDeck(m_removedCards, card);
	}
	else {
		RemoveCardFromDeck(m_cards, card);
	}
	
}

std::vector<PlayingCard>& Player::GetRemovedCards()
{
	return m_removedCards;
}

std::vector<PlayingCard>& Player::GetCards()
{
	return m_cards;
}

Card *Player::GetGrabbedCard() const
{
	return m_grabbedCard;
}

bool Player::IsGrabbingCard() const
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

void Player::SetIsPlayingAshes(bool isPlayingAshes)
{
	m_isPlayingAshes = isPlayingAshes;
}

bool Player::HasPlayedIllusion() const
{
	return m_hasPlayedIllusion;
}

bool Player::IsPlayingAshes() const
{
	return m_isPlayingAshes;
}

const Color Player::GetColor() const
{
	return m_color;
}

bool& Player::IsPlayingIllusion()
{
	return m_isPlayingIllusion;
}

void Player::Reset()
{
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;
	m_cards.clear();
	m_grabbedCard = nullptr;
}
