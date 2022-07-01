// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "core/timer.h"

Timer::Timer()
  : m_start_ticks(0)
  , m_pause_ticks(0)
  , m_paused(false)
  , m_started(false)
  , m_time(0)
{
}

void Timer::start()
{
    m_started     = true;
    m_paused      = false;
    m_start_ticks = SDL_GetTicks();
    m_pause_ticks = 0;
}

void Timer::stop()
{
    m_started     = false;
    m_paused      = false;
    m_start_ticks = 0;
    m_pause_ticks = 0;
}

void Timer::pause()
{
    if (m_started && !m_paused) {
        m_paused      = true;
        m_pause_ticks = SDL_GetTicks() - m_start_ticks;
        m_start_ticks = 0;
    }
}

void Timer::resume()
{
    if (m_started && m_paused) {
        m_paused      = false;
        m_start_ticks = SDL_GetTicks() - m_pause_ticks;
        m_pause_ticks = 0;
    }
}

uint32_t Timer::ticks()
{
    if (m_started) {
        if (m_paused) {
            m_time = m_pause_ticks;
        } else {
            m_time = SDL_GetTicks() - m_start_ticks;
        }
    }

    return m_time;
}

bool Timer::started() const
{
    return m_started;
}

bool Timer::paused() const
{
    return m_paused && m_started;
}
