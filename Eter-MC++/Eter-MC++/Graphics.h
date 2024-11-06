#ifndef GRAPHICS_H
#define GRAPHICS_H
#if defined linux
#define FONT_PATH   "../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#else 
#define FONT_PATH   "../../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#endif
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>
#include <cstring>
#include "Coordinates.h"
#include "Game.h"
#include "PlayingCard.h"

inline struct {
    std::string playerName;
    bool tournamentActive;
    bool mageDuelActive;
    bool elementalBattleActive;
    bool trainingActive;
}g_config;

class Graphics {
public:
    Graphics();
    ~Graphics();
    void drawText(const std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void drawTextBox(std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void drawButton(bool& active, const Coordinates& pos, int width, int height, std::string text, int fontSize);
    SDL_Renderer* GetRenderer();
    void setEvent(const SDL_Event &event);
    void setMousePos(const Coordinates& pos);
    bool isMouseInRect(const SDL_Rect& rect) const;
    bool drawLoginPage();
    void drawModeSelection();
    void drawCard(const PlayingCard& card);
    bool isTrainingActive();
    bool isMageDuelActive();
    bool isElementalActive();
    bool isTournamentActive();

    private:
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    TTF_Font *m_font;
    SDL_Color m_mainColor;
    SDL_Color m_accentColor;
    SDL_Event m_event;
    int m_mouseX, m_mouseY;
};


#endif //GRAPHICS_H
