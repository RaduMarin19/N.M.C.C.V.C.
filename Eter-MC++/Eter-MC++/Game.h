#pragma once
#include "Player.h"
#include "Graphics.h"
#include <iostream>
#include <algorithm>

#include <SDL.h>
#include <vector>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

class Game
{
public:
	Game();

	void run();

private:
	Player player1, player2;
};

