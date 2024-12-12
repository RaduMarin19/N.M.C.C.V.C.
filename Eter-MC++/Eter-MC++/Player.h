#pragma once
#include "PlayingCard.h"
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
	using SpellType = std::variant<std::monostate, MageType, ElementalType>;  //no value for training - other types for specific gamemodes

public:
	Player();
	Player(std::vector<PlayingCard>&& cards);
	Player(Player&&) = default;
	Player& operator=(Player&&) = default;

	void SetIllusionTexture(std::shared_ptr<CardTexture> texture);
	const CardTexture& GetIllusionTexture() const;

	void SetGrabbedCard(PlayingCard* grabbedCard);
	void AddCard(const PlayingCard& card);
	void SetSpellCard(SpellCard& spellCard);
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
	std::optional<SpellCard> m_spellCard;
	SpellType m_spell;
	std::shared_ptr<CardTexture> m_cardIllusion;

	PlayingCard *m_grabbedCard;
	bool m_isPlayingIllusion;
	bool m_isGrabbingCard;
	bool m_hasPlayedIllusion;
};

