//
// Created by mitza on 11/3/24.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H
#if defined linux  && SDL_VERSION_ATLEAST(2, 0, 8)
#define FONT_PATH   "../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#else 
#define FONT_PATH   "../../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"
#endif
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>
#include "Coordinates.h"

inline struct {
    std::string playerName;
}g_config;

class Graphics {
public:
    Graphics(SDL_Renderer*);
    void drawText(const std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void drawTextBox(std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void drawButton(bool& active, const Coordinates& pos, int width, int height, std::string text, int fontSize);
    void setEvent(const SDL_Event &event);
    void setMousePos(const Coordinates& pos);
    bool isMouseInRect(const SDL_Rect& rect) const;
    void draw();

    private:
    SDL_Renderer *m_renderer;
    TTF_Font *m_font;
    SDL_Color m_mainColor;
    SDL_Color m_accentColor;
    SDL_Event m_event;
    int m_mouseX, m_mouseY;
};


#endif //GRAPHICS_H
