#pragma once
#include "Player.h"
#include "Graphics.h"
#include "GameBoard.h"
#include "GameState.h"
#include "CardStatus.h"
#include "Graphics.h"
#include "ElementalType.h"

#if defined linux
#include "../Dependencies/JSON/json.hpp"
#else
#include "json.hpp"
#endif

#include <SDL.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <vector>
#include <stack>
#include <ranges>
#include "TokenCard.h"


#define TARGET_FRAME_TIME 16 // 16ms between frames (60~fps)
using json = nlohmann::json;

class Game
{
public:
	
	Game(const Game&) = delete;                  
	Game& operator=(const Game&) = delete;     
	Game(Game&&) = delete;                       
	Game& operator=(Game&&) = delete;

	static Game& GetInstance();

	void SetGameState(GameState state);
	GameState GetGameState() const;
	static void Run();

private:
	void IRun();
	Game();

	GameState m_currentState = GameState::WELCOME_SCREEN;
	GameState m_nextRoundState;
	int m_bestOf = -1;
	std::unique_ptr<GameBoard> m_board = nullptr;
	std::unique_ptr<Graphics> m_painter = nullptr;
	bool m_drawThisFrame;
	bool m_explosionTurn;
	bool m_isPlayingTournament;
	int m_quickModeTimer;
	std::vector<std::vector<TokenCard*>> m_miniArena;

	GameBoard::DeckType* m_selectedStack;

	void DrawPlayersCards(Player* player, bool isPlayersTurn, Player* otherPlayer);
	void DrawBoard();

	void HandleTournamentSelection();
	void HandleQuickModeSelection();
	void HandleModeSelection();
	bool HandleWin();
	void HandleBoardState();
	void HandleCardMovement(Player* player, Card& card);
	void PlayerTurn(Player& player,SDL_Rect& rect,const Coordinates& possiblePosition);
	void PlayRegularCard(Player& player,PlayingCard* pushCard, SDL_Rect& renderRect, const Coordinates& possiblePosition);
	void PlayWizardCard(Player& player, WizardCard* wizardCard, SDL_Rect& renderRect, const Coordinates& possiblePosition);
	void PlaySpellCard(Player& player, SpellCard* spellCard, SDL_Rect& renderRect, const Coordinates& possiblePosition);
	bool ExplosionTurn();

	void AddTokenToArena(const Coordinates& pos, const unsigned int& team);
	void CheckArenaForWin();

	void LoadSave();
	void SaveGame();
};