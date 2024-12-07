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
	Game();

	void SetGameState(GameState state);
	GameState GetGameState() const;
	void run();
private:

	GameState m_currentState = WELCOME_SCREEN;
	std::unique_ptr<GameBoard> m_board=nullptr;
	std::shared_ptr<Graphics> m_painter=nullptr;
	bool m_drawThisFrame;

	void HandleBoardState();
	void PlayerTurn(Player& player,SDL_Rect& rect,const Coordinates& possiblePosition);
};