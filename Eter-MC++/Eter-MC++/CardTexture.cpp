#include "CardTexture.h"
#include <iostream>

CardTexture::CardTexture()
{}

CardTexture::CardTexture(SDL_Renderer* renderer, const std::string& texturePath)
{
	m_rect.w = textureWidth;
	m_rect.h = textureHeight;

	//Load the surface image into memory
	SDL_Surface* surface = IMG_Load(texturePath.c_str());

	if (surface == NULL) {
		std::cerr << "Failed to load card texture: " << texturePath << std::endl
			<< "SDL_Error: " << SDL_GetError() << std::endl;
	}

	//Create a texture from the loaded surface
	m_texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (m_texture == NULL)
	{
		std::cerr << "Failed to create card texture: " << texturePath << std::endl
			<< "SDL_Error: " << SDL_GetError() << std::endl;
	}

	//After we created the texture, we can free the surface
	SDL_FreeSurface(surface);
}

CardTexture& CardTexture::operator=(const CardTexture &oth) {
	m_texture = oth.m_texture;
	m_rect = oth.m_rect;
	return *this;
}

SDL_Texture* CardTexture::GetTexture() const
{
	return m_texture;
}

SDL_Rect& CardTexture::GetRect()
{
	return m_rect;
}
