// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "particles/emitter.h"

#include "core/game.h"

Emitter::Emitter(Game& game)
  : m_game(game)
{
    m_clips[Particle::Effect::GAS]          = split(70, 6, 6 * 5);
    m_clips[Particle::Effect::MAGIC_CIRCLE] = split(32, 5, 8 * 5);
    m_clips[Particle::Effect::FIRE_BALL]    = split(32, 5, 8 * 5);
}

bool Emitter::initialize()
{
    if (!m_texture.load(m_game.window.renderer(), RESOLVE_RESOURCE("images/animations/sprites.png"))) {
        LOG_ERROR << "Failed to load texture" << std::endl;
        return false;
    }

    return true;
}

void Emitter::add_projectile_effect(Particle::Effect effect, Vector2D<float> origin, Vector2D<float> destination,
                                    std::function<void()> completion_fn)
{
    ParticleSequence sequence;
    sequence.destination   = destination;
    sequence.type          = ParticleSequence::PROJECTILE;
    sequence.completion_fn = completion_fn;

    Particle particle;
    particle.position = origin;
    particle.effect   = effect;
    particle.speed    = 2.0;
    particle.frame    = 0;
    particle.velocity = { 0.0, 0.0 };

    sequence.particles.push_back(particle);
    sequence.timer.start();

    m_sequences.push_back(sequence);
}

void Emitter::add_timed_effect(Particle::Effect effect, int timeout, Vector2D<float> origin,
                               std::function<void()> completion_fn)
{
    ParticleSequence sequence;
    sequence.duration      = timeout;
    sequence.type          = ParticleSequence::TIMED;
    sequence.completion_fn = completion_fn;

    Particle particle;
    particle.position = origin;
    particle.effect   = effect;
    particle.frame    = 0;

    sequence.particles.push_back(particle);
    sequence.timer.start();

    m_sequences.push_back(sequence);
}

void Emitter::update(uint32_t ticks)
{
    auto iter = m_sequences.begin();

    while (iter != m_sequences.end()) {
        auto& sequence = *iter;
        bool  complete = true;

        for (auto& particle : sequence.particles) {
            if (sequence.type == ParticleSequence::PROJECTILE) {

                int px = particle.position.x;
                int py = particle.position.y;
                int dx = sequence.destination.x;
                int dy = sequence.destination.y;

                particle.velocity.x = (dx - px) * particle.speed;
                particle.velocity.y = (dy - py) * particle.speed;

                LOG_DEBUG << "Projectile X: " << particle.position.x << "\n";
                LOG_DEBUG << "Projectile Y: " << particle.position.y << "\n";
                LOG_DEBUG << "Projectile Destination X: " << dx << "\n";
                LOG_DEBUG << "Projectile Destination Y: " << dy << "\n";

                const auto dt = ticks / 1000.0f;

                particle.position.x += particle.velocity.x * dt;
                particle.position.y += particle.velocity.y * dt;

                if (px != dx || py != dy) {
                    complete = false;
                }
            }

            particle.frame = (sequence.timer.ticks() * 60) / 1000 % 40;
        }

        if (sequence.type == ParticleSequence::TIMED) {
            if (sequence.timer.ticks() < sequence.duration) {
                complete = false;
            }
        }

        LOG_DEBUG << "Complete: " << complete << "\n";

        if (complete) {
            sequence.completion_fn();
            m_sequences.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Emitter::render(const SDL_Rect& camera)
{
    SDL_Rect rect;

    for (const auto& sequence : m_sequences) {
        for (const auto& particle : sequence.particles) {

            // rect.x = (particle.position.x - camera.x) - MAP_TILE_SIZE / 2;
            // rect.y = (particle.position.y - camera.y);

            // rect.w = MAP_TILE_SIZE;
            // rect.h = MAP_TILE_SIZE;

            // SDL_SetRenderDrawBlendMode(m_game.window.renderer(), SDL_BLENDMODE_BLEND);

            // SDL_SetRenderDrawColor(m_game.window.renderer(), 255, 0, 0, 50);
            // SDL_RenderFillRect(m_game.window.renderer(), &rect);

            m_texture.render((particle.position.x - SPRITE_SIZE / 2) - camera.x,
                             (particle.position.y - SPRITE_SIZE / 2) - camera.y,
                             &m_clips[particle.effect][particle.frame]);
        }

        rect.x = (sequence.destination.x - camera.x) - MAP_TILE_SIZE / 2;
        rect.y = (sequence.destination.y - camera.y);

        rect.w = MAP_TILE_SIZE;
        rect.h = MAP_TILE_SIZE;

        SDL_SetRenderDrawBlendMode(m_game.window.renderer(), SDL_BLENDMODE_BLEND);

        SDL_SetRenderDrawColor(m_game.window.renderer(), 255, 0, 0, 50);
        SDL_RenderFillRect(m_game.window.renderer(), &rect);
    }
}

Emitter::~Emitter()
{
}
