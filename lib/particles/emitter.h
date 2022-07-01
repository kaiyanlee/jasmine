// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <memory>
#include <vector>

#include "core/timer.h"
#include "graphics/texture.h"
#include "particles/particle.h"

class Game;

class Emitter
{
public:
    explicit Emitter(Game& game);
    ~Emitter();

public:
    // Initialize the particle engine.
    bool initialize();

    // Render the particles.
    void render(const SDL_Rect& camera);

    // Add a projectile effect that will finish after the destination has been reached.
    void add_projectile_effect(Particle::Effect effect, Vector2D<float> origin, Vector2D<float> destination,
                               std::function<void()> completion_fn);

    // Add a timed effect that will expire after a certain number of milliseconds.
    void add_timed_effect(Particle::Effect effect, int timeout, Vector2D<float> origin,
                          std::function<void()> completion_fn);

    void update(uint32_t ticks);

private:
    static const auto SPRITESHEET_COLUMNS = 6;
    static const auto SPRITESHEET_ROWS    = 75;
    static const auto SPRITE_SIZE         = 192;
    static const auto ANIMATION_COUNT     = 15;

    // Breaks up the spritesheet into a sequence of animated frames. The index is the starting row,
    // the columns represents the column count, and frames represents the number of frames.
    static const std::vector<SDL_Rect> split(int index, int columns, int frames)
    {
        std::vector<SDL_Rect> vec(frames);

        for (int i = 0; i < frames; ++i) {
            auto x = i > columns ? (i * SPRITE_SIZE) % columns : i * SPRITE_SIZE;
            auto y = i > columns ? SPRITE_SIZE * (index + (i / columns)) : SPRITE_SIZE * index;

            vec[i] = { x, y, SPRITE_SIZE, SPRITE_SIZE };
        }

        return vec;
    }

private:
    Game&   m_game;
    Texture m_texture;

    std::vector<SDL_Rect>         m_clips[ANIMATION_COUNT];
    std::vector<ParticleSequence> m_sequences;
};
