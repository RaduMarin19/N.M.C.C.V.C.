#pragma once
#include "Player.h"
#include "Graphics.h"
#include "Gameboard.h"
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
	enum GameState {
		WELCOME_SCREEN,
		MODE_SELECTION,
		TRAINING_MODE
	};

	void SetGameState(Game::GameState state);
	Game::GameState GetGameState() const;
	void run();
private:
	GameState m_currentState = WELCOME_SCREEN;
};

