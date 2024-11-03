//
// Created by mitza on 11/3/24.
//

#include "Graphics.h"

Graphics::Graphics(SDL_Renderer *renderer) {

    m_mainColor = {160, 220, 50, 255};
    m_accentColor = {40, 40, 80};

    m_renderer = renderer;
    TTF_Init();

    m_font = TTF_OpenFont(FONT_PATH, 40);
    if(!m_font) {
        printf("Unable to load font: '%s'!\n"
               "SDL2_ttf Error: %s\n", FONT_PATH, TTF_GetError());
        exit(-1);
    }
}

void Graphics::drawText(const std::string &buf, const Coordinates &pos, int fontSize, bool isCentered) {


    SDL_Texture *text = NULL;
    SDL_Rect textRect;

    textRect.x = pos.GetX();
    textRect.y = pos.GetY();

    SDL_Color textColor = m_accentColor;
    textColor.a = 255;

    SDL_Surface *textSurface = TTF_RenderText_Shaded(m_font, buf.c_str(), SDL_Color(255.f, 255.f, 255.f, 255.f), textColor);

    if(!textSurface) {
        printf("Unable to render text surface!\n"
               "SDL2_ttf Error: %s\n", TTF_GetError());
    } else {
        // Create texture from surface pixels
        text = SDL_CreateTextureFromSurface(m_renderer, textSurface);
        if(!text) {
            printf("Unable to create texture from rendered text!\n"
                   "SDL2 Error: %s\n", SDL_GetError());
            return;
        }
        // Get text dimensions
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        SDL_Rect outlineRect = textRect;
        outlineRect.w +=2;
        outlineRect.h +=2;
        outlineRect.x --;
        outlineRect.y --;

        if(isCentered) {
            textRect.x -= textSurface->w /2;
            outlineRect.x -= textSurface->w /2;
        }

        SDL_SetRenderDrawColor(m_renderer, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);

        SDL_RenderDrawRect(m_renderer, &outlineRect);

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

        SDL_RenderCopy(m_renderer, text, NULL, &textRect);

        SDL_FreeSurface(textSurface);
    }
}

void Graphics::setEvent(const SDL_Event &event) {
    m_event = event;
}
