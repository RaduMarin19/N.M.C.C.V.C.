#pragma once
#include "Coordinates.h"
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

    CardStatus pushNewCard(const PlayingCard& otherCard);
    void setTable(short tableSize);
    short getTableSize() const;
    void setGameMode(const GameMode& mode);
    void generatePlayerCards(const GameMode& mode);
    GameBoard(SDL_Renderer* renderer);

    unsigned short nextCardId();

    bool getCardAtPosition(const Coordinates& coordinates, PlayingCard& card) const;
    const std::unordered_set<Coordinates, Coordinates::Hash>& GetPossiblePositions();
    const std::vector<PlayingCard> GetPlayedCards() const;
    std::unordered_map<Coordinates, std::deque<PlayingCard>, Coordinates::Hash>& GetPlayedPositions();
    std::unordered_set<Coordinates, Coordinates::Hash>& GetHoles();

    Player* getPlayerRed();
    Player* getPlayerBlue();

    bool canUseExplosion();
    bool didExplode() const;
    bool validateExplosion();
    void explode();
    void updateBoardMask();
    void printExplosionMask();
    std::unique_ptr<CardTexture>& GetExplosionBoardTexture();
    CardTexture* GetExplosionSprite(const int& offset);

    const CardTexture& getBlueIllusionTexture() const;
    const CardTexture& getRedIllusionTexture() const;
    void CheckStatus(GameState& gameMode);

    void returnCardToDeck(PlayingCard& card);

    void setIsBluePlayer(bool player);
    bool isBluePlayer() const;

    bool verifyNeighbours(const std::array<std::array<uint8_t, 3>, 3>& mask, int x, int y);

    void updateBoardCenter();

    unsigned int getCenterX() const;
    unsigned int getCenterY() const;

    ExplosionCard* getExplosion();
    std::optional<std::pair<SpellCard, SpellCard>>& GetSpells();
    void initializeExplosion();

    void clear();
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

    std::list<PlayingCard> m_blueRemovedCards;
    std::list<PlayingCard> m_redRemovedCards;
    std::optional<std::pair<SpellCard, SpellCard>> m_spells;

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

    void testPossiblePosition(short x, short y);
    bool CheckRows(GameState& gameState);
    bool CheckColumns(GameState& gameState);
    bool CheckDiagonals(GameState& gameState);
    bool CheckScore(GameState& gameState);

    void DeleteCardAtPosition(const Coordinates& boardPosition);
    void CreateHoleAtPosition(const Coordinates& boardPosition);
    void ReturnCardAtPosition(const Coordinates& boardPosition);

    void LoadTextures(SDL_Renderer* renderer);

    void GenerateTrainingCards();
    void GenerateElementalCards();
};
