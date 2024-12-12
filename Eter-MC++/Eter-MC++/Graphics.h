#ifndef GRAPHICS_H
#define GRAPHICS_H
#if defined linux
#define FONT_PATH   "../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#else 
#define FONT_PATH   "../../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#endif

#include "Coordinates.h"
#include "PlayingCard.h"
#include "config.h"

#include<iostream>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>
#include <cstring>

inline struct {
    std::string playerName;
    bool tournamentActive;
    bool mageDuelActive;
    bool elementalBattleActive;
    bool trainingActive;
    bool quickMatchActive;
}g_config;

class Graphics {
public:
    Graphics();
    ~Graphics();
    void DrawText(const std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void DrawTextBox(std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void DrawButton(bool& active, const Coordinates& pos, int width, int height, std::string text, int fontSize);
    void DrawTexturedRect(const SDL_Rect& rect, SDL_Texture* texture);
    SDL_Renderer* GetRenderer();
    void SetEvent(const SDL_Event &event);
    void SetMousePos(const Coordinates& pos);
    Coordinates GetMousePos();
    void ResetGameModes();
    bool IsMouseInRect(const SDL_Rect& rect) const;
    bool DrawLoginPage();
    void DrawModeSelection();
	void DrawTournamentModeSelection();
    void DrawCard(const Card& card, SDL_Texture* cardTexture);
    bool IsTrainingActive();
    bool IsMageDuelActive();
    bool IsElementalActive();
    bool IsTournamentActive();
    bool IsQuickMatchActive();
    bool IsPressingLeftClick();
    bool IsPressingRightClick();

private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font *m_font;
    SDL_Color m_mainColor;
    SDL_Color m_accentColor;
    SDL_Event m_event;
    int m_mouseX, m_mouseY;
    bool m_isPressingLeftClick;
    bool m_isPressingRightClick;
};


#endif //GRAPHICS_H
