// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include <string>

// Texture is a wrapper over SDL_Texture. It can be used to render
// images or text (clipped or otherwise).
class Texture
{
public:
    // Create a new Texture.
    explicit Texture();

    // Texture destructor.
    ~Texture();

public:
    // Load texture from an image or data file.
    bool load(SDL_Renderer* renderer, const char* file_path);

    // Load from text and wrap it using a fixed width. The width and height arguments will return
    // the size of the rendered text.
    bool load_from_text(SDL_Renderer* renderer, const char* text, TTF_Font* font, const SDL_Color& color);

    // Cleanup/reset the texture.
    void cleanup();

    // Render the texture.
    void render(int x, int y, const SDL_Rect* src = nullptr);

    // Transform and then render texture.
    void render_transform(int x, int y);

    // Render the texture using color modulation.
    void render_color(int x, int y, const SDL_Color color, const SDL_Rect* src = nullptr);

    // Set a custom blend mode.
    void set_blend_mode(SDL_BlendMode mode);

    // Set the alpha value (0-255).
    void    set_alpha(uint8_t alpha);
    uint8_t alpha() const;

    // Set the value to scale the texture when rendering via render_transform().
    void  set_scale(float scale);
    float scale() const;

    // Set the rotation angle when rendering via render_transform().
    void  set_rotation(float rotation);
    float rotation() const;

    // Return the width of the texture.
    int width() const;

    // Return the height of the texture.
    int height() const;

private:
    SDL_Texture*  m_texture;
    SDL_Renderer* m_renderer;
    int           m_width;
    int           m_height;
    uint8_t       m_alpha;
    float         m_scale;
    float         m_rotation;
    SDL_Rect      m_dest;
};
