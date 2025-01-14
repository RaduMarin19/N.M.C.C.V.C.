#pragma once

enum class GameState : short
{
	WELCOME_SCREEN,
	MODE_SELECTION,
	TRAINING_MODE,
	ELEMENTAL_BATTLE,
	MAGE_DUEL,
	TOURNAMENT,
	QUICK_MODE,
	MAGE_ELEMENTAL,
	BLUE_PLAYER_WON,
	RED_PLAYER_WON,
	TIE
};