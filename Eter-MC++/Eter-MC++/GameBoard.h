#pragma once
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
#include "ExplosionCard.h"

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
    std::unordered_map<Coordinates, std::deque<PlayingCard>, Coordinates>& GetPlayedPositions();

    Player *getPlayerRed();
    Player *getPlayerBlue();

    bool canUseExplosion();
    bool didExplode() const;
    bool validateExplosion();
    void explode();
    void updateBoardMask();
    void printExplosionMask();
    std::shared_ptr<CardTexture>& GetExplosionBoardTexture();
    CardTexture* GetExplosionSprite(const int& offset);
    std::array<std::array<ExplosionType, 3>, 3> GetExplosionMask();

    std::shared_ptr<CardTexture>& getBlueIllusionTexture();
    std::shared_ptr<CardTexture>& getRedIllusionTexture();
    void checkStatus(GameState& gameMode);
    void rotateExplosionMask();

    void returnCardToDeck(PlayingCard& card);

    void setIsBluePlayer(bool player);
    bool isBluePlayer();

    
    bool verifyNeighbours(const std::array<std::array<uint8_t, 3>, 3>& mask, int x, int y);
    
	void updateBoardCenter();

	unsigned int getCenterX();
	unsigned int getCenterY();

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

	std::shared_ptr<CardTexture> m_explosionBoard;
    std::shared_ptr<CardTexture> m_blueCardIllusion;
    std::shared_ptr<CardTexture> m_redCardIllusion;

    bool m_isBluePlayer;
    bool m_exploded = false;
    std::array<std::array<ExplosionType, 3>, 3> m_explosionMask;
    std::array<std::array<uint8_t, 3>, 3> m_boardMask;

	ExplosionCard* m_explosion = nullptr;

	unsigned int m_centerX;
	unsigned int m_centerY;


    void testPossiblePosition(short x, short y);
};
