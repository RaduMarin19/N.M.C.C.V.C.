#define _CRT_SECURE_NO_WARNINGS

#include "Graphics.h"

#include <format>

Graphics::Graphics() {

    //Initialize the color pallete, currently "magic numbers"
    m_mainColor = {160, 220, 50, 255};
    m_accentColor = {40, 40, 80};

    //Initialize random seed
    srand(static_cast<unsigned int>(time(NULL)));

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL Initialization Failed");
    }

#if defined linux && SDL_VERSION_ATLEAST(2, 0, 8)
        // Disable compositor bypass
        if (!SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR, "0")) {
            std::cerr << "SDL cannot disable compositor bypass!" << std::endl;
            throw std::runtime_error("SDL Hint Setting Failed");
        }
#endif

    // Create window
    m_window = SDL_CreateWindow("Basic C++ SDL project",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    SCREEN_WIDTH, SCREEN_HEIGHT,
    SDL_WINDOW_SHOWN);
    if (!m_window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL Window Creation Failed");
    }

    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("SDL Renderer Creation Failed");
    }

        // Initialize TTF
        if (TTF_Init() == -1) {
            std::cerr << "TTF_Init Failed! TTF_Error: " << TTF_GetError() << std::endl;
            throw std::runtime_error("TTF Initialization Failed");
        }

    if(m_font == nullptr)
        m_font = TTF_OpenFont(FONT_PATH, 40);

    if(!m_font) {
        printf("Unable to load font: '%s'!\n"
               "SDL2_ttf Error: %s\n", FONT_PATH, TTF_GetError());
        exit(-1);
    }
}

Graphics::~Graphics()
{
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    TTF_Quit();
    SDL_Quit();
}

void Graphics::DrawText(const std::string &buf, const Coordinates &pos, int fontSize, bool isCentered, SDL_Color textColor) {

    //Initialize a texture and rectangle for the text
    SDL_Texture *texture = NULL;
    SDL_Rect textRect;

    textRect.x = pos.GetX();
    textRect.y = pos.GetY();

    SDL_Color texColor = m_accentColor;
    texColor.a = 255;

    //Try to load our text buffer onto a surface
    SDL_Surface *textSurface = TTF_RenderText_Shaded(m_font, buf.c_str(), textColor, texColor);

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
        // Get text dimensions
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        //Create an "outline", a rectangle that's one pixel larger than our text rectangle
        SDL_Rect outlineRect = textRect;
        outlineRect.w +=2;
        outlineRect.h +=2;
        outlineRect.x --;
        outlineRect.y --;

        //If the centered flag is set, shift the rectangles by half it's width
        if(isCentered) {
            textRect.x -= textSurface->w /2;
            outlineRect.x -= textSurface->w /2;

            textRect.y -= textSurface->h /2;
            outlineRect.y -= textSurface->h /2;
        }

        //Render our rectangles and the texture onto the screen
        SDL_SetRenderDrawColor(m_renderer, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);

        SDL_RenderDrawRect(m_renderer, &outlineRect);

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

        SDL_RenderCopy(m_renderer, texture, NULL, &textRect);

        //Delete the created surface
        SDL_FreeSurface(textSurface);
    }
}

void Graphics::DrawTextBox(std::string &buf, const Coordinates &pos, int fontSize, bool isCentered) {

    //Initialize a texture and rectangle for the text
    SDL_Texture *texture = NULL;
    SDL_Rect textRect;

    textRect.x = pos.GetX();
    textRect.y = pos.GetY();

    SDL_Color textColor = m_accentColor;
    textColor.a = 30;

    static char *buffer = nullptr;
    bool isPlaceholder = false;

    //If the given buffer is empty, replace it with placeholder text
    if(buf.empty()) {
        char placeHolder[] = "Enter text...";
        if(buffer == nullptr)
            buffer = new char[strlen(placeHolder) + 1];
        strcpy(buffer, placeHolder);
        isPlaceholder = true;
    } else {
        delete buffer;
        buffer = new char[strlen(buf.c_str()) + 1];
        strcpy(buffer, buf.c_str());
    }

    //Try to load our text buffer onto a surface
    SDL_Surface *textSurface = TTF_RenderText_Shaded(m_font, buffer, isPlaceholder ? SDL_Color(80, 80, 80, 240) : SDL_Color(255.f, 255.f, 255.f, 255.f), textColor);

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
        // Get text dimensions
        textRect.w = textSurface->w;
        textRect.h = textSurface->h;

        //Create an "outline", a rectangle that's one pixel larger than our text rectangle
        SDL_Rect outlineRect = textRect;
        outlineRect.w = (textSurface->w / strlen(buffer)) * 20;
        outlineRect.h +=2;
        outlineRect.x --;
        outlineRect.y --;

        //If the centered flag is set, shift the rectangles by half it's width
        if(isCentered) {
            textRect.x -= ((textSurface->w / strlen(buffer)) * 20) / 2;
            outlineRect.x -= ((textSurface->w / strlen(buffer)) * 20) / 2;

            textRect.y -= textSurface->h /2;
            outlineRect.y -= textSurface->h /2;
        }

        //Typing logic, if the mouse is on the element, we can type in it
        //TODO: Change this logic to be on click instead
        if(this->IsMouseInRect(outlineRect)) {
            if(m_event.type == SDL_KEYDOWN) {
                //Get the key ASCII id
                unsigned short key = m_event.key.keysym.sym;

                //If we pressed backspace, delete a character instead of adding it
                //TODO: This logic should delete multiple characters if backspace is held down
                if(key == SDLK_BACKSPACE) {
                    buf = buf.substr(0, buf.size() - 1);
                } else if(buf.length() < 20) buf += static_cast<char>(key);
            }
            SDL_SetRenderDrawColor(m_renderer, m_mainColor.r, m_mainColor.g, m_mainColor.b, m_mainColor.a);
        } else SDL_SetRenderDrawColor(m_renderer, m_accentColor.r, m_accentColor.g, m_accentColor.b, m_accentColor.a);

        //Render our rectangles and the texture onto the screen
        SDL_RenderDrawRect(m_renderer, &outlineRect);

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

        SDL_RenderCopy(m_renderer, texture, NULL, &textRect);

        //Delete the created surface
        SDL_FreeSurface(textSurface);
    }
}

void Graphics::DrawButton(bool &active, const Coordinates &pos, int width, int height, std::string text, int fontSize) {

    //Initialize a texture and rectangle for the text
    SDL_Rect buttonRect = {pos.GetX(), pos.GetY(), width, height};

    SDL_Texture *texture = NULL;
    SDL_Color chosenColor;

    //Is the mouse hovering our button?
    if(IsMouseInRect(buttonRect)) {
        //If we are hovering and we pressed on it, invert its state
        if(m_event.type == SDL_MOUSEBUTTONDOWN) {
            if(m_event.button.button == SDL_BUTTON_LEFT) {
                active = !active;
            }
            //Set the button color depending on the mouse action
            chosenColor = m_mainColor;
        } else chosenColor = m_accentColor;
    } else {
        chosenColor = m_accentColor;
    }

    SDL_SetRenderDrawColor(m_renderer, chosenColor.r, chosenColor.g, chosenColor.b, chosenColor.a);

    //Try to load our text buffer onto a surface
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

    //Create an "outline", a rectangle that's one pixel larger than our text rectangle
    SDL_Rect outlineRect = buttonRect;
    outlineRect.w += 2;
    outlineRect.h +=2;
    outlineRect.x --;
    outlineRect.y --;

    SDL_SetRenderDrawColor(m_renderer, m_accentColor.r, m_accentColor.g, m_accentColor.b, m_accentColor.a);

    //Render our rectangles and the texture onto the screen
    SDL_RenderDrawRect(m_renderer, &outlineRect);

    SDL_RenderCopy(m_renderer, texture, NULL, &buttonRect);

    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);

    //Delete the created surface
    SDL_FreeSurface(textSurface);
}

SDL_Renderer* Graphics::GetRenderer()
{
    return m_renderer;
}

void Graphics::SetEvent(const SDL_Event &event) {
    m_event = event;
    SDL_GetMouseState(&m_mouseX, &m_mouseY);

    if(event.button.button == SDL_BUTTON_LEFT && event.type == SDL_MOUSEBUTTONDOWN) {
        m_isPressingLeftClick = true;
    }

    if(event.button.button == SDL_BUTTON_RIGHT && event.type == SDL_MOUSEBUTTONDOWN) {
        m_isPressingRightClick = true;
    }
    if(event.type == SDL_MOUSEBUTTONUP) {
        m_isPressingLeftClick = false;
    }
}

void Graphics::SetMousePos(const Coordinates &pos) {
    m_mouseX = pos.GetX();
    m_mouseY = pos.GetY();
}

Coordinates Graphics::GetMousePos() {
    return {m_mouseX, m_mouseY};
}

bool Graphics::IsMouseInRect(const SDL_Rect &rect) const {
    return (m_mouseX > rect.x &&m_mouseX < rect.x + rect.w
        && m_mouseY > rect.y && m_mouseY < rect.y + rect.h);
}

bool Graphics::DrawLoginPage() {

    //Prepare the context for drawing
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    //Draw our elements onto the screen, a text item, a text box and a button
    DrawText("Welcome to ETER!", {SCREEN_WIDTH / 2, 50}, 14, true);
    DrawTextBox(g_config.playerName, {SCREEN_WIDTH / 2, 250}, 14, true);

    bool buttonActive = false;
    DrawButton(buttonActive, { SCREEN_WIDTH / 2 - 50, 350 }, 100, 40, "Log in!", 14);

    // Present the updated render
    SDL_RenderPresent(m_renderer);

    //If the user pressed the button return true, otherwise return false;
    return buttonActive;

}

void Graphics::DrawModeSelection(GameState& gameState) {
    //Prepare the context for drawing
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    //Draw our elements onto the screen, a text item and our buttons
    DrawText("Choose Your Game Mode", { SCREEN_WIDTH / 2, 50 }, 18, true);
    bool tournamentActive = false;
    bool mageDuelActive = false;
    bool elementalBattleActive = false;
    bool trainingActive = false;
    bool quickMatchActive = false;

    // Draw and check each button
    DrawButton(tournamentActive,      { SCREEN_WIDTH / 2 - 75, 150 }, 150, 40, "Tournament", 14);
    DrawButton(mageDuelActive,        { SCREEN_WIDTH / 2 - 75, 200 }, 150, 40, "Mage Duel", 14);
    DrawButton(elementalBattleActive, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Elemental Battle", 14);
    DrawButton(trainingActive,        { SCREEN_WIDTH / 2 - 75, 300 }, 150, 40, "Training", 14);
    DrawButton(quickMatchActive,      { SCREEN_WIDTH / 2 - 75, 350 }, 150, 40, "Quick Match", 14);

    if (tournamentActive) {
        gameState = GameState::TOURNAMENT;
    }
    if (mageDuelActive) {
        gameState = GameState::MAGE_DUEL;
    }
    if (elementalBattleActive) {
        gameState = GameState::ELEMENTAL_BATTLE;
    }
    if (trainingActive) {
        gameState = GameState::TRAINING_MODE;
    }
    if (quickMatchActive) {
        gameState = GameState::QUICK_MODE;
    }

    SDL_RenderPresent(m_renderer);
}


void Graphics::DrawQuickModeSelection(GameState& gameState, int& timer)
{
    //Prepare the context for drawing
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    //Draw our elements onto the screen, a text item and our buttons
    DrawText("Choose Your Game Mode for Quick Match!", { SCREEN_WIDTH / 2, 50 }, 18, true);

    bool mageDuelActive = false;
    bool elementalBattleActive = false;
    bool trainingActive = false;
    bool mageElementalActive = false;
    bool tournamentActive = false;

    // Draw and check each button
    DrawButton(mageDuelActive,        { SCREEN_WIDTH / 2 - 75, 200 }, 150, 40, "Mage Duel", 14);
    DrawButton(elementalBattleActive, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Elemental Battle", 14);
    DrawButton(trainingActive,        { SCREEN_WIDTH / 2 - 75, 300 }, 150, 40, "Training", 14);
    DrawButton(mageElementalActive,   { SCREEN_WIDTH / 2 - 75, 350 }, 150, 40, "Mage Elemental", 14);
    DrawButton(tournamentActive,      { SCREEN_WIDTH / 2 - 75, 400 }, 150, 40, "Tournament", 14);
    DrawSlider(timer, 30, 120, 30, { SCREEN_WIDTH / 2 - 75, 450 }, 150, 40);

    if (mageDuelActive) {
        gameState = GameState::MAGE_DUEL;
    }
    if (elementalBattleActive) {
        gameState = GameState::ELEMENTAL_BATTLE;
    }
    if (trainingActive) {
        gameState = GameState::TRAINING_MODE;
    }
    if (mageElementalActive) {
        gameState = GameState::MAGE_ELEMENTAL;
    }
    if (tournamentActive) {
        gameState = GameState::TOURNAMENT;
    }

    SDL_RenderPresent(m_renderer);
}

void Graphics::DrawTournamentModeSelection(GameState& gameState)
{
    //Prepare the context for drawing
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);

    //Draw our elements onto the screen, a text item and our buttons
    DrawText("Choose Your Game Mode for Tournament", { SCREEN_WIDTH / 2, 50 }, 18, true);
    
    bool mageDuelActive = false;
    bool elementalBattleActive = false;
    bool trainingActive = false;
    bool mageElementalActive = false;

    // Draw and check each button
    DrawButton(mageDuelActive, { SCREEN_WIDTH / 2 - 75, 200 }, 150, 40, "Mage Duel", 14);
    DrawButton(elementalBattleActive, { SCREEN_WIDTH / 2 - 75, 250 }, 150, 40, "Elemental Battle", 14);
    DrawButton(trainingActive, { SCREEN_WIDTH / 2 - 75, 300 }, 150, 40, "Training", 14);
    DrawButton(mageElementalActive, { SCREEN_WIDTH / 2 - 75, 350 }, 150, 40, "Mage Elemental", 14);

    if (mageDuelActive) {
        gameState = GameState::MAGE_DUEL;
    }
    if (elementalBattleActive) {
        gameState = GameState::ELEMENTAL_BATTLE;
    }
    if (trainingActive) {
        gameState = GameState::TRAINING_MODE;
    }
    if (mageElementalActive) {
        gameState = GameState::MAGE_ELEMENTAL;
    }

    SDL_RenderPresent(m_renderer);
}

void Graphics::DrawCard(const Card& card, SDL_Texture* cardTexture)
{
    // Ensure the card has a valid texture
    if (!card.GetTexture() || !cardTexture) {
        std::cerr << "Error: PlayingCard has no valid texture!\n";
        return;
    }

    // Get the texture and source rectangle from the card
    SDL_Rect srcRect = card.GetTexture()->GetRect();

    // Get the card's position
    Coordinates cardPos = card.GetCoordinates();

    // Destination rectangle based on card's position
    SDL_Rect destRect;
    destRect.x = cardPos.GetX();
    destRect.y = cardPos.GetY();
    destRect.w = srcRect.w;
    destRect.h = srcRect.h;

    // Optional: Scale the card if desired
    // For example, to scale to 128x128:
    // destRect.w = 128;
    // destRect.h = 128;

    // Render the card
    if (SDL_RenderCopy(m_renderer, cardTexture, NULL, &destRect) != 0) {
        std::cerr << "SDL_RenderCopy Error: " << SDL_GetError() << std::endl;
    }
}

void Graphics::DrawTexturedRect(const SDL_Rect& rect, SDL_Texture* texture) {
    if (!texture) {
        std::cerr << "Error: Texture has no valid texture!\n";
        return;
    }

    if (SDL_RenderCopy(m_renderer, texture, NULL, &rect) != 0) {
        std::cerr << "SDL_RenderCopy Error: " << SDL_GetError() << std::endl;
    }
}

void Graphics::DrawTimer(unsigned int seconds, const Coordinates &pos, int fontSize) {

    std::string formattedTime = std::format("{:02}:{:02}", seconds / 60, seconds % 60);

    SDL_Color color;
    if(seconds >= 60)
        color = {255, 255, 255};
    else
        color = {180, 0, 0};
    this->DrawText(formattedTime, pos, fontSize, true, color);
}

void Graphics::DrawSlider(int& value, const int& minValue, const int& maxValue, const int& step, const Coordinates& pos, const int& width, const int& height) {
    value = minValue;

}

bool Graphics::IsPressingLeftClick() {
    return this->m_isPressingLeftClick;
}

bool Graphics::IsPressingRightClick() {
    return this->m_isPressingRightClick;
}

