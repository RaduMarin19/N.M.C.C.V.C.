#ifndef GRAPHICS_H
#define GRAPHICS_H
#if defined linux
#define FONT_PATH   "../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#else 
#define FONT_PATH   "../../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#endif

#include "Coordinates.h"
#include "PlayingCard.h"
#include "GameState.h"
#include "config.h"

#include <iostream>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

inline struct {
    std::string playerName;
}g_config;

class Graphics {
public:
    Graphics();
    ~Graphics();

    Graphics(const Graphics&) = delete;
    Graphics& operator=(const Graphics&) = delete;
    Graphics(Graphics&&) = delete;
    Graphics& operator=(Graphics&&) = delete;

    void DrawText(const std::string& buf, const Coordinates& pos, int fontSize, bool isCentered, SDL_Color textColor = {255,255,255});
    void DrawTextBox(std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void DrawButton(bool& active, const Coordinates& pos, int width, int height, std::string text, int fontSize);
    void DrawTexturedRect(const SDL_Rect& rect, SDL_Texture* texture);
    void DrawTimer(unsigned int seconds, const Coordinates& pos, int fontSize);
    void DrawSlider(int& value, const int& minValue, const int& maxValue, const int& step, const Coordinates& pos, const int& width, const int& height);

    SDL_Renderer* GetRenderer();
    void SetEvent(const SDL_Event &event);
    void resetEvent();

    void SetMousePos(const Coordinates& pos);
    Coordinates GetMousePos();

    bool IsMouseInRect(const SDL_Rect& rect) const;

    bool DrawLoginPage();
    void DrawModeSelection(GameState& gameState);
	void DrawTournamentModeSelection(GameState& gameState);
	void DrawQuickModeSelection(GameState& gameState, int& timer);

    void DrawCard(const Card& card, SDL_Texture* cardTexture);

    bool IsPressingLeftClick();
    bool IsPressingRightClick();
    bool IsDraggingMouse();
private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    std::array<TTF_Font*, 2> m_font;
    SDL_Color m_mainColor;
    SDL_Color m_accentColor;
    SDL_Event m_event;
    int m_mouseX, m_mouseY;
    bool m_isDraggingMouse;
    bool m_isPressingLeftClick;
    bool m_isPressingRightClick;
    bool m_PressedKeyThisFrame;
    unsigned short m_PressedKey;
};


#endif //GRAPHICS_H
