//
// Created by mitza on 11/3/24.
//

#ifndef GRAPHICS_H
#define GRAPHICS_H
#define FONT_PATH   "../Eter-MC++/Eter-MC++/Dependencies/assets/DroidSansMono.ttf"

#include <SDL_render.h>
#include <SDL_ttf.h>
#include <string>
#include "Coordinates.h"

class Graphics {
public:
    Graphics(SDL_Renderer*);
    void drawText(const std::string& buf, const Coordinates& pos, int fontSize, bool isCentered);
    void setEvent(const SDL_Event &event);

    private:
    SDL_Renderer *m_renderer;
    TTF_Font *m_font;
    SDL_Color m_mainColor;
    SDL_Color m_accentColor;
    SDL_Event m_event;
};



#endif //GRAPHICS_H
