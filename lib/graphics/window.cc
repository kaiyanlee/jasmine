// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "graphics/window.h"

#include "core/common.h"
#include "core/logger.h"

Window::Window()
  : m_window(nullptr)
  , m_renderer(nullptr)
{
}

bool Window::create(const char* title)
{
    // Our window flags.
    auto flags = SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP;

    // Position of the window.
    auto position = SDL_WINDOWPOS_CENTERED;

    // Width and height are negative because we are a fullscreen application.
    auto width  = -1;
    auto height = -1;

    // Do not use linear filtering (it makes things blurry).
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
        LOG_ERROR << "Failed to set render scale quality" << std::endl;
    }

    // Force enable vertical synchronization.
    if (!SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")) {
        LOG_ERROR << "Failed to set vsync hint" << std::endl;
    }

    m_window = SDL_CreateWindow(title, position, position, width, height, flags);

    if (m_window == nullptr) {
        LOG_ERROR << "Failed to create window: " << SDL_GetError() << std::endl;
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (m_renderer == nullptr) {
        LOG_ERROR << "Failed to create hardware accelerated renderer: " << SDL_GetError() << std::endl;
        m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
    }

    if (m_renderer == nullptr) {
        LOG_ERROR << "Failed to renderer: " << SDL_GetError() << std::endl;
        return false;
    } else {
        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 0);
    }

    // Scale everything.
    SDL_RenderSetScale(m_renderer, WINDOW_SCALE, WINDOW_SCALE);

    // Set the window size.
    SDL_GetWindowSize(m_window, &m_width, &m_height);

    // Scale the window size.
    m_width /= WINDOW_SCALE;
    m_height /= WINDOW_SCALE;

    return true;
}

int Window::width() const
{
    return m_width;
}

int Window::height() const
{
    return m_height;
}

SDL_Renderer* Window::renderer() const
{
    return m_renderer;
}

Window::~Window()
{
    LOG_DEBUG << "Destroying renderer" << std::endl;

    if (m_renderer != nullptr) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    LOG_DEBUG << "Destroying window" << std::endl;

    if (m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}
