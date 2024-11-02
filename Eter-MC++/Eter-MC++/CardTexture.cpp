#include "CardTexture.h"
#include <iostream>

CardTexture::CardTexture()
{}

CardTexture::CardTexture(SDL_Renderer* renderer, const std::string& texturePath)
{
	m_rect.w = textureWidth;
	m_rect.h = textureHeight;

	SDL_Surface* surface = IMG_Load(texturePath.c_str());

	if (surface == NULL) {
		std::cerr << "Failed to load card texture: " << texturePath << std::endl
			<< "SDL_Error: " << SDL_GetError() << std::endl;
	}

	m_texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (m_texture == NULL)
	{
		std::cerr << "Failed to create card texture: " << texturePath << std::endl
			<< "SDL_Error: " << SDL_GetError() << std::endl;
	}

	SDL_FreeSurface(surface);
}

SDL_Texture* CardTexture::getTexture() const
{
	return m_texture;
}

SDL_Rect CardTexture::getRect() const
{
	return m_rect;
}
