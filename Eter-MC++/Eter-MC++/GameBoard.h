﻿#pragma once
#include "Coordinates.h"
#include "PlayingCard.h"
#include "CardTexture.h"
#include "Player.h"
#include "GameState.h"
#include "CardStatus.h"
#include "ExplosionType.h"

#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <array>
#include <vector>
#include <time.h>
#include <cstdlib>
#include <variant>
#include <random>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 750

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
    void generateRandomExplosion();
	GameBoard(SDL_Renderer* renderer);

    unsigned short nextCardId();

    bool getCardAtPosition(const Coordinates& coordinates, PlayingCard& card) const;
    const std::unordered_set<Coordinates, Coordinates>& GetPossiblePositions();
    const std::vector<PlayingCard> GetPlayedCards() const;
    std::unordered_map<Coordinates, std::stack<PlayingCard>, Coordinates>& GetPlayedPositions();

    Player *getPlayerRed();
    Player *getPlayerBlue();

    CardTexture *getBlueIllusionTexture();
    bool canUseExplosion();
    CardTexture *getRedIllusionTexture();
    void checkStatus(GameState& gameMode);
    void rotateExplosionMask();

    void returnCardToDeck(PlayingCard& card);

    void setIsBluePlayer(bool player);
    bool isBluePlayer();

    bool didExplode() const;
    bool verifyNeighbours(const std::array<std::array<uint8_t, 3>, 3>& mask, int x, int y);
    void updateBoardMask();
	bool validateExplosion();
    void explode();

	void updateBoardCenter();

	unsigned int getCenterX();
	unsigned int getCenterY();

    void printExplosionMask();

	CardTexture *GetExplosionBoardTexture();
	CardTexture *GetExplosionSprite(const int& offset);
	std::array<std::array<ExplosionType, 3>, 3> GetExplosionMask();

    void clear();
protected:
    GameMode m_gameMode;

private:
	unsigned short m_cardId{ 0 };
    short m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 };
    short tableSize{ 3 }; // it varies between 3 and 4 depending on game mode

	static const unsigned int m_playerHandPadding {30};

    std::unordered_map<Coordinates, std::stack<PlayingCard>, Coordinates> m_positions;
    std::unordered_set<Coordinates, Coordinates> m_possiblePositions;
    std::unordered_set<Coordinates, Coordinates> m_holes;

    Player m_playerBlue;
    Player m_playerRed;

    std::vector<CardTexture> m_blueCards;
    std::vector<CardTexture> m_redCards;
    std::vector<CardTexture> m_explosionSprites;
	CardTexture *m_explosionBoard;

    CardTexture *m_blueCardIllusion;
    CardTexture *m_redCardIllusion;

    bool m_isBluePlayer;
    bool m_exploded = false;
    std::array<std::array<ExplosionType, 3>, 3> m_explosionMask;
    std::array<std::array<uint8_t, 3>, 3> m_boardMask;

	unsigned int m_centerX;
	unsigned int m_centerY;


    void testPossiblePosition(short x, short y);
};
