#pragma once
#include "Player.h"
#include "Graphics.h"
#include "GameBoard.h"
#include "GameState.h"
#include <iostream>
#include <algorithm>

#include <SDL.h>
#include <vector>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
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

};

