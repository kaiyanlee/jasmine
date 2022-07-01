// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL_timer.h>

#include <iomanip>
#include <iostream>

static auto last_count = SDL_GetPerformanceCounter();

inline auto ms_elapsed_since_last_call()
{
    auto now_count = SDL_GetPerformanceCounter();
    auto now_ms    = ((now_count - last_count) * 1000) / SDL_GetPerformanceFrequency();

    last_count = now_count;

    return now_ms;
}

#define LOG_INFO std::cout

#ifdef DEBUG_MODE
#define LOG_DEBUG \
    std::cout << std::setw(8) << ms_elapsed_since_last_call() << " " << __FUNCTION__ << ":" << __LINE__ << " "
#else
// Noop; this should never execute.
#define LOG_DEBUG \
    while (false) \
    std::cout
#endif

#define LOG_ERROR \
    std::cout << std::setw(8) << ms_elapsed_since_last_call() << " " << __FUNCTION__ << ":" << __LINE__ << " "
