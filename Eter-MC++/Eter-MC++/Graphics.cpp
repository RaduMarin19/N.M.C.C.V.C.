//
// Created by mitza on 11/3/24.
//

#define _CRT_SECURE_NO_WARNINGS

#include "Graphics.h"

#include <cstring>

#include "Game.h"

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

            textRect.y -= textSurface->h /2;
            outlineRect.y -= textSurface->h /2;
        }

        SDL_SetRenderDrawColor(m_renderer, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);

        SDL_RenderDrawRect(m_renderer, &outlineRect);

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

        SDL_RenderCopy(m_renderer, text, NULL, &textRect);

        SDL_FreeSurface(textSurface);
    }
}

void Graphics::drawTextBox(std::string &buf, const Coordinates &pos, int fontSize, bool isCentered) {

    SDL_Texture *text = NULL;
    SDL_Rect textRect;

    textRect.x = pos.GetX();
    textRect.y = pos.GetY();

    SDL_Color textColor = m_accentColor;
    textColor.a = 30;

    char *buffer;
    bool isPlaceholder = false;

    if(buf.empty()) {
        char placeHolder[] = "Enter text...";
        buffer = new char[strlen(placeHolder) + 1];
        strcpy(buffer, placeHolder);
        isPlaceholder = true;
    } else {
        buffer = new char[strlen(buf.c_str()) + 1];
        strcpy(buffer, buf.c_str());
    }

    SDL_Surface *textSurface = TTF_RenderText_Shaded(m_font, buffer, isPlaceholder ? SDL_Color(80, 80, 80, 240) : SDL_Color(255.f, 255.f, 255.f, 255.f), textColor);

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
        outlineRect.w = (textSurface->w / strlen(buffer)) * 20;
        outlineRect.h +=2;
        outlineRect.x --;
        outlineRect.y --;

        if(isCentered) {
            textRect.x -= ((textSurface->w / strlen(buffer)) * 20) / 2;
            outlineRect.x -= ((textSurface->w / strlen(buffer)) * 20) / 2;

            textRect.y -= textSurface->h /2;
            outlineRect.y -= textSurface->h /2;
        }

        if(this->isMouseInRect(outlineRect)) {
            if(m_event.type == SDL_KEYDOWN) {
                unsigned short key = m_event.key.keysym.sym;

                if(key == SDLK_BACKSPACE) {
                    buf = buf.substr(0, buf.size() - 1);
                } else if(buf.length() < 20) buf += static_cast<char>(key);
            }
            SDL_SetRenderDrawColor(m_renderer, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);
        } else SDL_SetRenderDrawColor(m_renderer, m_accentColor.r, m_accentColor.g, m_accentColor.b, m_accentColor.a);

        SDL_RenderDrawRect(m_renderer, &outlineRect);

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

        SDL_RenderCopy(m_renderer, text, NULL, &textRect);

        SDL_FreeSurface(textSurface);
    }
}

void Graphics::drawButton(bool &active, const Coordinates &pos, int width, int height, std::string text, int fontSize) {

    SDL_Rect buttonRect = {pos.GetX(), pos.GetY(), width, height};

    SDL_Texture *texture = NULL;
    SDL_Color chosenColor;

    if(isMouseInRect(buttonRect)) {
        if(m_event.type == SDL_MOUSEBUTTONDOWN) {
            if(m_event.button.button == SDL_BUTTON_LEFT) {
                active = !active;
            }
            chosenColor = m_mainColor;
        } else chosenColor = m_accentColor;
    } else {
        chosenColor = m_accentColor;
    }

    SDL_SetRenderDrawColor(m_renderer, chosenColor.r, chosenColor.g, chosenColor.b, chosenColor.a);

    SDL_Surface *textSurface = TTF_RenderText_Shaded(m_font, text.c_str(), SDL_Color(255.f, 255.f, 255.f, 255.f), chosenColor);

    if(!textSurface) {
        printf("Unable to render text surface!\n"
               "SDL2_ttf Error: %s\n", TTF_GetError());
    } else {
        // Create texture from surface pixels
        texture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
        if(!texture) {
            printf("Unable to create texture from rendered text!\n"
                   "SDL2 Error: %s\n", SDL_GetError());
            return;
        }
    }

    SDL_Rect outlineRect = buttonRect;
    outlineRect.w += 2;
    outlineRect.h +=2;
    outlineRect.x --;
    outlineRect.y --;

    SDL_SetRenderDrawColor(m_renderer, m_accentColor.r, m_accentColor.g, m_accentColor.b, m_accentColor.a);

    SDL_RenderDrawRect(m_renderer, &outlineRect);

    SDL_RenderCopy(m_renderer, texture, NULL, &buttonRect);

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

    SDL_FreeSurface(textSurface);
}

void Graphics::setEvent(const SDL_Event &event) {
    m_event = event;
    SDL_GetMouseState(&m_mouseX, &m_mouseY);
}

void Graphics::setMousePos(const Coordinates &pos) {
    m_mouseX = pos.GetX();
    m_mouseY = pos.GetY();
}

bool Graphics::isMouseInRect(const SDL_Rect &rect) const {
    return (m_mouseX > rect.x &&m_mouseX < rect.x + rect.w
&& m_mouseY > rect.y && m_mouseY < rect.y + rect.h);
}

void Graphics::draw() {

    drawText("Welcome to ETER!", {SCREEN_WIDTH / 2, 50}, 14, true);
    drawTextBox(g_config.playerName, {SCREEN_WIDTH / 2, 250}, 14, true);
}
