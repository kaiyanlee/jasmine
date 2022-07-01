// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is cleanup software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "graphics/texture.h"

#include <SDL_image.h>

#include "core/logger.h"

Texture::Texture()
  : m_texture(nullptr)
  , m_renderer(nullptr)
  , m_width(0)
  , m_height(0)
  , m_alpha(255)
  , m_scale(1.0)
  , m_rotation(0.0)
{
}

bool Texture::load(SDL_Renderer* renderer, const char* file_path)
{
    cleanup();

    if (m_renderer != renderer) {
        m_renderer = renderer;
    }

    SDL_Surface* surface = IMG_Load(file_path);

    if (surface == nullptr) {
        LOG_ERROR << "Failed to load image: " << IMG_GetError() << std::endl;
        return false;
    }

    if (SDL_SetSurfaceRLE(surface, 1) != 0) {
        return false;
    }

    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    if (m_texture == nullptr) {
        LOG_ERROR << "Failed to load texture: " << SDL_GetError() << std::endl;
        return false;
    }

    m_width  = surface->w;
    m_height = surface->h;

    m_dest.w = m_width;
    m_dest.h = m_height;

    SDL_FreeSurface(surface);

    return true;
}

bool Texture::load_from_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color)
{
    cleanup();

    if (m_renderer != renderer) {
        m_renderer = renderer;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);

    if (surface == nullptr) {
        LOG_ERROR << "Failed to render text surface: " << TTF_GetError() << std::endl;
        return false;
    }

    m_texture = SDL_CreateTextureFromSurface(m_renderer, surface);

    if (m_texture == nullptr) {
        LOG_ERROR << "Failed to create texture from text: " << SDL_GetError() << std::endl;
        return false;
    }

    m_width  = surface->w;
    m_height = surface->h;

    m_dest.w = m_width;
    m_dest.h = m_height;

    SDL_FreeSurface(surface);

    return true;
}

void Texture::cleanup()
{
    LOG_DEBUG << "Texture destroyed..."
              << "\n";

    if (m_texture != nullptr) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
}

void Texture::set_blend_mode(SDL_BlendMode mode)
{
    SDL_SetTextureBlendMode(m_texture, mode);
}

void Texture::set_alpha(uint8_t alpha)
{
    if (m_alpha != alpha) {
        SDL_SetTextureAlphaMod(m_texture, alpha);
        m_alpha = alpha;
    }
}

uint8_t Texture::alpha() const
{
    return m_alpha;
}

int Texture::width() const
{
    return m_width;
}

int Texture::height() const
{
    return m_height;
}

float Texture::scale() const
{
    return m_scale;
}

void Texture::set_scale(float scale)
{
    m_scale  = scale;
    m_dest.w = static_cast<int>(m_width * m_scale);
    m_dest.h = static_cast<int>(m_height * m_scale);
}

float Texture::rotation() const
{
    return m_rotation;
}

void Texture::set_rotation(float rotation)
{
    m_rotation = rotation;
}

void Texture::render(int x, int y, const SDL_Rect* src)
{
    m_dest.x = x;
    m_dest.y = y;

    if (src != nullptr) {
        m_dest.w = src->w;
        m_dest.h = src->h;
    }

    SDL_RenderCopy(m_renderer, m_texture, src, &m_dest);
}

void Texture::render_transform(int x, int y)
{
    m_dest.x = x;
    m_dest.y = y;

    SDL_RenderCopyEx(m_renderer, m_texture, nullptr, &m_dest, m_rotation, nullptr, SDL_FLIP_NONE);
}

void Texture::render_color(int x, int y, const SDL_Color color, const SDL_Rect* src)
{
    m_dest.x = x;
    m_dest.y = y;

    if (src != nullptr) {
        m_dest.w = src->w;
        m_dest.h = src->h;
    }

    SDL_SetTextureColorMod(m_texture, color.r, color.g, color.b);
    SDL_RenderCopy(m_renderer, m_texture, src, &m_dest);
}

Texture::~Texture()
{
    cleanup();
}
