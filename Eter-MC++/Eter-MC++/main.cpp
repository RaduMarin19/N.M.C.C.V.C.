#include "Game.h"

int main(int argc, char* argv[])
{
	//Step into main game loop
	Game::GetInstance().run();

	return 0;
}