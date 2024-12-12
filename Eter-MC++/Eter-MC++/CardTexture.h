#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>

#define textureWidth 128
#define textureHeight 128

class CardTexture
{
public:
	CardTexture();
	CardTexture(SDL_Renderer* renderer, const std::string& texturePath);
	CardTexture& operator= (const CardTexture&);
	SDL_Texture* GetTexture() const;
	SDL_Rect& GetRect();

private:
	SDL_Rect m_rect;
	SDL_Texture* m_texture=nullptr;
};

