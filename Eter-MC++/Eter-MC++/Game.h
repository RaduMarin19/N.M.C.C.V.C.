#pragma once
#include "Player.h"
#include "Graphics.h"
#include "GameBoard.h"
#include <iostream>
#include <algorithm>

#include <SDL.h>
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define TARGET_FRAME_TIME 16 // 16ms between frames (60~fps)

class Game
{
public:
	Game();

	enum GameState : short 
	{
		WELCOME_SCREEN,
		MODE_SELECTION,
		TRAINING_MODE,
		ELEMENTAL_BATTLE,
		MAGE_DUEL,
		TOURNAMENT,
		QUICK_MODE
	};

	void SetGameState(Game::GameState state);

	Game::GameState GetGameState() const;
	void run();

private:
	GameState m_currentState = WELCOME_SCREEN;

};

