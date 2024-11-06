﻿#pragma once
#include "Coordinates.h"
#include "PlayingCard.h"
#include "CardTexture.h"
#include "Game.h"
#include "Player.h"

#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <vector>


class GameBoard
{
public:
	enum class GameMode : short
    {
        Training,
        MageDuel,
        Elemental,
        Tournament,
        Quick
    };

    GameBoard();
    void pushNewCard(PlayingCard other);
    void setTable(short tableSize);
	void setGameMode(const GameMode& mode);
    void generatePlayerCards(const GameMode& mode);
	GameBoard(SDL_Renderer* renderer);

    unsigned short nextCardId();

    bool getCardAtPosition(const Coordinates& coordinates, PlayingCard& card) const;

    Player *getPlayerRed();
    Player *getPlayerBlue();

protected:
    GameMode m_gameMode{ GameMode::Training };

private:
	unsigned short m_cardId{ 0 };
    short m_minX{ 0 }, m_maxX{ 0 }, m_minY{ 0 }, m_maxY{ 0 };
    short tableSize{ 3 }; // it varies between 3 and 4 depending on game mode

    static const int coordinatePadding { 50 };

    std::unordered_map<Coordinates, std::stack<PlayingCard>, Coordinates> m_positions;
    std::unordered_set<Coordinates, Coordinates> m_possiblePositions;

    Player m_playerBlue;
    Player m_playerRed;

    std::vector<CardTexture> m_blueCards;
    std::vector<CardTexture> m_redCards;
    std::vector<CardTexture> m_explosions;

    void testPossiblePosition(short x, short y);
};
