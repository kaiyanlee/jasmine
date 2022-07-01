// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL2/SDL.h>

// Timer allows you to access and monitor elapsed time using
// SDL_GetTicks.
class Timer
{
public:
    // Create a new Timer.
    Timer();

public:
    // Return the number of ticks since start() was called.
    uint32_t ticks();

    // Start the timer.
    void start();

    // Stop the timer.
    void stop();

    // Resume the timer.
    void resume();

    // Pause the timer.
    void pause();

    // Return true if the timer has been started.
    bool started() const;

    // Return true if the timer is paused.
    bool paused() const;

private:
    uint32_t m_start_ticks;
    uint32_t m_pause_ticks;
    bool     m_paused;
    bool     m_started;
    uint32_t m_time;
};
