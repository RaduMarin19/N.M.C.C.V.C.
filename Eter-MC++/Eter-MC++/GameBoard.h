﻿#pragma once
#include "Coordinates.h"
#include "PlayingCard.h"
#include "CardTexture.h"
#include "Player.h"
#include "GameState.h"
#include "CardStatus.h"
#include "ExplosionType.h"
#include "ExplosionCard.h"
#include "config.h"

#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <array>
#include <vector>
#include <time.h>
#include <iostream>
#include <cstdlib>
#include <variant>
#include <random>

class GameBoard
{
public:
	enum GameMode : short
    {
        Training,
        MageDuel,
        Elemental,
        Tournament,
        QuickMode
    };

    GameBoard();
    CardStatus pushNewCard(const PlayingCard& otherCard);
    void setTable(short tableSize);
	short getTableSize() const;
	void setGameMode(const GameMode& mode);
    void generatePlayerCards(const GameMode& mode);
	GameBoard(SDL_Renderer* renderer);

    unsigned short nextCardId();

    bool getCardAtPosition(const Coordinates& coordinates, PlayingCard& card) const;
    const std::unordered_set<Coordinates, Coordinates>& GetPossiblePositions();
    const std::vector<PlayingCard> GetPlayedCards() const;
    std::unordered_map<Coordinates, std::deque<PlayingCard>, Coordinates>& GetPlayedPositions();
    std::unordered_set<Coordinates, Coordinates>& GetHoles();

    Player *getPlayerRed();
    Player *getPlayerBlue();

    bool canUseExplosion();
    bool didExplode() const;
    bool validateExplosion();
    void explode();
    void updateBoardMask();
    void printExplosionMask();
    std::unique_ptr<CardTexture>& GetExplosionBoardTexture();
    CardTexture* GetExplosionSprite(const int& offset);

    std::unique_ptr<CardTexture>& getBlueIllusionTexture();
    std::unique_ptr<CardTexture>& getRedIllusionTexture();
    void CheckStatus(GameState& gameMode);

    void returnCardToDeck(PlayingCard& card);

    void setIsBluePlayer(bool player);
    bool isBluePlayer() const;

    
    bool verifyNeighbours(const std::array<std::array<uint8_t, 3>, 3>& mask, int x, int y);
    
	void updateBoardCenter();

	unsigned int getCenterX() const;
	unsigned int getCenterY() const;

	ExplosionCard* getExplosion();
	void initializeExplosion();

    void clear();
protected:
    GameMode m_gameMode;

private:
	unsigned short m_cardId{ 0 };
    short m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 };
    short m_tableSize{ 3 }; // it varies between 3 and 4 depending on game mode

	static const unsigned int m_playerHandPadding {30};

    std::unordered_map<Coordinates, std::deque<PlayingCard>, Coordinates> m_positions;
    std::unordered_set<Coordinates, Coordinates> m_possiblePositions;
    std::unordered_set<Coordinates, Coordinates> m_holes;

    Player m_playerBlue;
    Player m_playerRed;

    std::vector<CardTexture> m_blueCards;
    std::vector<CardTexture> m_redCards;
    std::vector<CardTexture> m_explosionSprites;

	std::unique_ptr<CardTexture> m_explosionBoard;
    std::unique_ptr<CardTexture> m_blueCardIllusion;
    std::unique_ptr<CardTexture> m_redCardIllusion;

    bool m_isBluePlayer;
    bool m_exploded = false;
    std::array<std::array<ExplosionType, 3>, 3> m_explosionMask;
    std::array<std::array<uint8_t, 3>, 3> m_boardMask;

	ExplosionCard* m_explosion = nullptr;

	unsigned int m_centerX;
	unsigned int m_centerY;


    void testPossiblePosition(short x, short y);
    bool CheckRows(GameState& gameState);
    bool CheckColumns(GameState& gameState);
    bool CheckDiagonals(GameState& gameState);
    bool CheckScore(GameState& gameState);
};
