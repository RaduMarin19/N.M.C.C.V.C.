#include "Player.h"

Player::Player() {
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;
	m_grabbedCard = nullptr;
	m_timeRemaining = 0;
	m_deltaTime = 0;

	std::cout << "Called Player() constructor\n";
}

Player::Player(std::vector<PlayingCard>& cards) : m_cards(std::move(cards)) {
	m_isGrabbingCard = false;
	m_hasPlayedIllusion = false;
	m_isPlayingIllusion = false;
	m_isPlayingAshes = false;
	m_timeRemaining = 0;
	m_deltaTime = 0;

	m_color = m_cards.back().GetColor();

	m_grabbedCard = nullptr;

	std::cout << "Called Player(vector) constructor\n";
}

void Player::SetColor(Color color) {
	m_color = color;
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

void Player::SetWizard(WizardType wizard, CardTexture* texture, Coordinates position, unsigned short cardId)
{
	m_wizard.release();

	m_wizard = std::make_unique<WizardCard>(position, texture, wizard, cardId);
}

const std::unique_ptr<WizardCard>& Player::GetWizardCard() const
{
	return m_wizard;
}

void Player::RemoveWizard()
{
	m_wizard.release();
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

const std::vector<PlayingCard>& Player::GetRemovedCards() const
{
	return m_removedCards;
}

std::vector<PlayingCard>& Player::GetRemovedCards()
{
	return m_removedCards;
}

const std::vector<PlayingCard>& Player::GetCards() const
{
	return m_cards;
}

std::vector<PlayingCard>& Player::GetCards()
{
	return m_cards;
}

std::stack<PlayingCard> & Player::GetOrderOfPlayedCards() {
	return this->m_OrderOfPlayedCards;
}

void Player::AddCardToOrderStack(PlayingCard card) {
	this->m_OrderOfPlayedCards.push(card);
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

void Player::ClearCards() {
	m_cards.clear();
}

void Player::ClearRemovedCards() {
	m_removedCards.clear();
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

void Player::LoadRemovedCard(PlayingCard& card)
{
	m_removedCards.emplace_back(card);
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

void Player::setTimeRemaining(unsigned int timeRemaining) {
	m_timeRemaining = timeRemaining;
}

unsigned int Player::GetTimeRemaining() const {
	return m_timeRemaining;
}

void Player::setDeltaTime(unsigned int deltaTime) {
	m_deltaTime = deltaTime;
}

unsigned int Player::GetDeltaTime() const {
	return m_deltaTime;
}

bool Player::getIsPlayingIllusion() const {
	return m_isPlayingIllusion;
}
