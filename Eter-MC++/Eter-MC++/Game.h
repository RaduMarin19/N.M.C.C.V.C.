#pragma once
#include "Player.h"
#include "Graphics.h"
#include "GameBoard.h"
#include "GameState.h"
#include "CardStatus.h"
#include "Graphics.h"

#include <iostream>
#include <algorithm>
#include <SDL.h>
#include <memory>
#include <vector>


#define TARGET_FRAME_TIME 16 // 16ms between frames (60~fps)

class Game
{
public:
	
	static Game& GetInstance();

	void SetGameState(GameState state);
	GameState GetGameState() const;
	void run();

private:

	Game();
	static Game gameInstance;

	GameState m_currentState = WELCOME_SCREEN;
	std::unique_ptr<GameBoard> m_board = nullptr;
	std::unique_ptr<Graphics> m_painter = nullptr;
	bool m_drawThisFrame;
	bool m_explosionTurn = false;

	void DrawPlayersCards(Player* player,bool isPlayersTurn);
	void DrawBoard();

	void HandleBoardState();
	void HandleCardMovement(Player* player, PlayingCard& card);
	void PlayerTurn(Player& player,SDL_Rect& rect,const Coordinates& possiblePosition);
	bool ExplosionTurn();
};