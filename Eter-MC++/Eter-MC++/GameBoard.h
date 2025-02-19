﻿#pragma once
#include "Coordinates.h"
#include "Color.h"
#include "PlayingCard.h"
#include "CardTexture.h"
#include "Player.h"
#include "GameState.h"
#include "CardStatus.h"
#include "ExplosionType.h"
#include "ExplosionCard.h"
#include "TokenCard.h"
#include "Random.h"
#include "config.h"
#include "SpellCard.h"
#include "ElementalType.h"
#if defined linux
#include "../Dependencies/JSON/json.hpp"
#else
#include "json.hpp"
#endif

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdlib>
#include <ranges>

class GameBoard
{
public:
    using SpellsType = std::optional<std::pair<std::unique_ptr<SpellCard>, std::unique_ptr<SpellCard>>>;
    using DeckType = std::deque<PlayingCard>;
    using PlayingBoard = std::unordered_map<Coordinates, DeckType, Coordinates::Hash>;
    using GamePositions = std::unordered_set<Coordinates, Coordinates::Hash>;

public:

    GameBoard(SDL_Renderer* renderer);
    GameBoard(const GameBoard&) = delete; 
    GameBoard& operator=(const GameBoard&) = delete; 
    GameBoard(GameBoard&&) = delete; 
    GameBoard& operator=(GameBoard&&) = delete; 
    ~GameBoard() = default; 

    CardStatus PushNewCard(const PlayingCard& otherCard);
    void SetTable(short tableSize);
    short GetTableSize() const;
    void GeneratePlayerCards(GameState& gameState);

    unsigned short NextCardId();
    const Color GetCardColorAtPosition(const Coordinates& boardPosition) const;

    const GamePositions& GetPossiblePositions();
    const std::vector<const PlayingCard*> GetPlayedCards() const;
    std::unordered_map<Coordinates, DeckType, Coordinates::Hash>& GetPlayedPositions();
    DeckType& GetCardsAtPosition(const Coordinates& position);
    GamePositions& GetHoles();
    bool ValidateBoardAfterEffect(ExplosionCard *card);
    void SetValidatedExplosion(ExplosionCard *card);
    ExplosionCard* GetValidatedExplosion();

    Player* GetPlayerRed();
    Player* GetPlayerBlue();

    void IncreaseRoundsWon(GameState state);
    short GetBlueRoundsWon() const;
    short GetRedRoundsWon() const;
    void SetBlueRoundsWon(short value);
    void SetRedRoundsWon(short value);

    bool CanUseExplosion();
    bool DidExplode() const;
    void Explode();
    void UpdateBoardMask();
    std::unique_ptr<CardTexture>& GetExplosionBoardTexture();
    CardTexture* GetExplosionSprite(const int& offset);

    bool RemoveIllusion(const Coordinates& boardPosition);
    void SetCanCoverIllusion(bool canCoverIllusion);
    bool WhirlPool(const Coordinates& position);
    bool Hurricane(const Coordinates& position);    

    void ReturnCoveredCards();
    void RemoveSpell(SpellCard* spell);

    const CardTexture& GetBlueIllusionTexture() const;
    const CardTexture& GetRedIllusionTexture() const;
    void CheckStatus(GameState& gameMode);

    bool ChangeCardValue(PlayingCard& card, short valueChange);
    void ResetCardValue(PlayingCard& card);
    bool MoveEdgeRow(short row);

    void ReturnCardToDeck(Card& card);
    void DeleteCardAtPosition(const Coordinates& boardPosition);
    void ReturnTopCardAtPosition(const Coordinates& boardPosition);
    bool RemoveRow(short row,Color color); //removes a row if has at least 3 decks and at least one card has the color specified

    bool IsBluePlayer() const;
    bool IsPlayingMirage() const;

    void SetIsPlayingMirage(bool isPlayingMirage);
    void FixBorders(const Coordinates& position);
    void ResetPossiblePositions();
    bool ShouldResetPositions() const;

    void SetShouldResetPositions(bool shouldResetPositions);

    void ChangeTurn();

    bool VerifyNeighbours(const std::array<std::array<uint8_t, 3>, 3>& mask, int x, int y);

    short GetFixedX() const;
    short GetFixedY() const;

    void UpdateBoardCenter();
    bool SetBlockedRow(short row);
    void SetBoundPosition(const Coordinates& position);
    bool MoveStackToEmptyPosition(const Coordinates& position);
    void MoveStack(const Coordinates& target, const Coordinates& emptyPosition);
    void SetIsPlayingCoverOpponent(bool isPlayingCoverOpponent);
    bool IsPositionEmpty(const Coordinates& position) const;

    bool MoveStacksToEmptyPosition(const Coordinates& stack1, const Coordinates& stack2);
    bool WaterFall(const Coordinates& position);
    bool Flurry(const Coordinates& position);
    Coordinates GetUnTranslatedPosition(const Coordinates& position) const;

    unsigned int GetCenterX() const;
    unsigned int GetCenterY() const;

    unsigned int GetMinX() const;
    unsigned int GetMinY() const;
    unsigned int GetMaxX() const;
    unsigned int GetMaxY() const;
    
    void CreateHoleAtPosition(const Coordinates& boardPosition);
    ExplosionCard* GetExplosion();
    SpellsType& GetSpells();
    void InitializeExplosion();

    PlayingCard GenerateEterCard(Color color);

    void SaveState(nlohmann::json& json) const;
    void LoadState(const nlohmann::json& json);
    void Clear();
    void ResetRound(GameState gameState);

    void SetPlayingQuickMatch(bool val);
    bool GetPlayingQuickMatch() const;

    TokenCard* GetTokenCard(const unsigned int& team) const;

private:
    unsigned short m_cardId{ 0 };
    short m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 };
    short m_tableSize{ 3 }; // it varies between 3 and 4 depending on game mode

    static const unsigned int m_playerHandPadding{ 30 };

    PlayingBoard m_positions;
    GamePositions m_possiblePositions;
    GamePositions m_holes;
    GamePositions m_positionsToErase; //keeps positions to erase in the next turn
    short m_blockedRow;
    Coordinates* m_boundPosition;

    bool m_canCoverIllusion;
    bool m_isPlayingMirage;
    bool m_isPlayingCoverOpponent;

    bool m_isPlayingQuickMatch;

    bool m_isMinXFixed;
    bool m_isMaxXFixed;
    bool m_isMinYFixed;
    bool m_isMaxYFixed;

    bool m_shouldResetPositions;

    SpellsType m_spells;

    Player m_playerBlue;
    Player m_playerRed;

    short m_blueRoundsWon;
    short m_redRoundsWon;

    std::vector<CardTexture> m_blueCardTextures;
    std::vector<CardTexture> m_redCardTextures;
    std::vector<CardTexture> m_elementalCardTextures;
    std::vector<CardTexture> m_mageCardTextures;
    std::vector<CardTexture> m_explosionSprites;

    std::array<std::unique_ptr<CardTexture>, 2> m_explosionBoard;
    std::shared_ptr<CardTexture> m_blueCardIllusion;
    std::shared_ptr<CardTexture> m_redCardIllusion;

    std::array<std::unique_ptr<TokenCard>, 2> m_tokenCards;

    bool m_isBluePlayer;
    bool m_exploded = false;
    std::array<std::array<ExplosionType, 3>, 3> m_explosionMask;
    std::array<std::array<uint8_t, 3>, 3> m_boardMask;

    std::unique_ptr<ExplosionCard> m_explosion;
    std::unique_ptr<ExplosionCard> m_validatedExplosion;

    unsigned int m_centerX;
    unsigned int m_centerY;

    void TestPossiblePosition(short x, short y);
    void TestPossiblePositions(const Coordinates& boardPosition);
    bool CheckRows(GameState& gameState);
    bool CheckColumns(GameState& gameState);
    bool CheckDiagonals(GameState& gameState);
    bool CheckScore(GameState& gameState);

    void ReturnCardAtPosition(PlayingCard& card);

    void LoadTextures(SDL_Renderer* renderer);

    void GenerateTrainingCards();
    void GenerateElementalCards(bool shouldGenerateNewElements);
    void GenerateMageDuelCards();
    void GenerateMageElementalCards(bool shouldGenerateNewElements);

    void InitializeWizard(Player& player, short wizardId);
    void InitializeWizardCards(short randomIndex1, short randomIndex2);
    void InitializeSpellCards(short spellCardId1,short spellCardId2,int offset);
};
