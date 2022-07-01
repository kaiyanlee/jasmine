// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL2/SDL.h>

#include <functional>
#include <optional>

#include "core/math.h"
#include "core/timer.h"

struct Particle
{
    enum Effect
    {
        GAS          = 0,
        MAGIC_CIRCLE = 1,
        FIRE_BALL    = 2,
    };

    // The particle effect.
    Effect effect;

    // The current animation frame.
    int frame;

    // The color of the particle (optional).
    std::optional<SDL_Color> color;

    // The alpha of the system.
    uint8_t alpha;

    // The scale of the particle.
    float scale;

    // The coordinates of the particle.
    Vector2D<float> position;

    // The velocity of the particle.
    Vector2D<float> velocity;

    // The speed of the particle (for projectiles);
    float speed;
};

struct ParticleSequence
{
    enum Type
    {
        TIMED,
        PROJECTILE,
    };

    // The duration of the effect (for animations).
    uint32_t duration;

    // The current time elapsed.
    Timer timer;

    // The destination (for projectiles).
    Vector2D<float> destination;

    // The callback called when the sequence is complete.
    std::function<void()> completion_fn;

    // The particles in this group.
    std::vector<Particle> particles;

    // The type of sequence.
    Type type;
};
