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
#include "Random.h"
#include "config.h"
#include "SpellCard.h"
#include "ElementalType.h"
#include "MageType.h"
#include "GameMode.h"

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <stack>
#include <array>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <variant>

class GameBoard
{

public:
    using SpellsType = std::optional<std::pair<std::unique_ptr<SpellCard>, std::unique_ptr<SpellCard>>>;
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
    void SetGameMode(const GameMode& mode);
    void GeneratePlayerCards(const GameMode& mode);

    unsigned short NextCardId();
    const Color GetCardColorAtPosition(const Coordinates& boardPosition) const;

    const std::unordered_set<Coordinates, Coordinates::Hash>& GetPossiblePositions();
    const std::vector<PlayingCard> GetPlayedCards() const;
    std::unordered_map<Coordinates, std::deque<PlayingCard>, Coordinates::Hash>& GetPlayedPositions();
    std::deque<PlayingCard>& GetCardsAtPosition(const Coordinates& position);
    std::unordered_set<Coordinates, Coordinates::Hash>& GetHoles();

    Player* GetPlayerRed();
    Player* GetPlayerBlue();

    bool CanUseExplosion();
    bool DidExplode() const;
    bool ValidateExplosion();
    void Explode();
    void UpdateBoardMask();
    void PrintExplosionMask();
    std::unique_ptr<CardTexture>& GetExplosionBoardTexture();
    CardTexture* GetExplosionSprite(const int& offset);

    bool RemoveIllusion(const Coordinates& boardPosition);
    void ReturnCoveredCards();
    void RemoveSpell(SpellCard* spell);

    const CardTexture& GetBlueIllusionTexture() const;
    const CardTexture& GetRedIllusionTexture() const;
    void CheckStatus(GameState& gameMode);

    void ReturnCardToDeck(Card& card);
    void DeleteCardAtPosition(const Coordinates& boardPosition);

    bool IsBluePlayer() const;

    void ChangeTurn();

    bool VerifyNeighbours(const std::array<std::array<uint8_t, 3>, 3>& mask, int x, int y);

    void UpdateBoardCenter();

    unsigned int GetCenterX() const;
    unsigned int GetCenterY() const;

    ExplosionCard* GetExplosion();
    SpellsType& GetSpells();
    void InitializeExplosion();

    void Clear();
protected:
    GameMode m_gameMode;

private:
    unsigned short m_cardId{ 0 };
    short m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 };
    short m_tableSize{ 3 }; // it varies between 3 and 4 depending on game mode

    static const unsigned int m_playerHandPadding{ 30 };

    std::unordered_map<Coordinates, std::deque<PlayingCard>, Coordinates::Hash> m_positions;
    std::unordered_set<Coordinates, Coordinates::Hash> m_possiblePositions;
    std::unordered_set<Coordinates, Coordinates::Hash> m_holes;

    SpellsType m_spells;

    Player m_playerBlue;
    Player m_playerRed;

    std::vector<CardTexture> m_blueCardTextures;
    std::vector<CardTexture> m_redCardTextures;
    std::vector<CardTexture> m_elementalCardTextures;
    std::vector<CardTexture> m_mageCardTextures;
    std::vector<CardTexture> m_explosionSprites;

    std::unique_ptr<CardTexture> m_explosionBoard;
    std::shared_ptr<CardTexture> m_blueCardIllusion;
    std::shared_ptr<CardTexture> m_redCardIllusion;

    bool m_isBluePlayer;
    bool m_exploded = false;
    std::array<std::array<ExplosionType, 3>, 3> m_explosionMask;
    std::array<std::array<uint8_t, 3>, 3> m_boardMask;

    std::unique_ptr<ExplosionCard> m_explosion;

    unsigned int m_centerX;
    unsigned int m_centerY;

    void TestPossiblePosition(short x, short y);
    void TestPossiblePositions(const Coordinates& boardPosition);
    bool CheckRows(GameState& gameState);
    bool CheckColumns(GameState& gameState);
    bool CheckDiagonals(GameState& gameState);
    bool CheckScore(GameState& gameState);

    
    void CreateHoleAtPosition(const Coordinates& boardPosition);
    void ReturnTopCardAtPosition(const Coordinates& boardPosition);

    void ReturnCardAtPosition(PlayingCard& card);

    void LoadTextures(SDL_Renderer* renderer);

    void GenerateTrainingCards();
    void GenerateElementalCards();
};
