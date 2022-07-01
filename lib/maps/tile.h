// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL2/SDL.h>

#include <cstdint>

// Tile represents a grid tile.
class Tile
{
public:
    enum Layer
    {
        BACKGROUND_1 = 0, // The main static background.
        BACKGROUND_2 = 1, // Static layer on top of background (ex. ladders).
        FOREGROUND   = 2, // Dynamic & collision-detected.
    };

    enum Sprite
    {
        SMALL_GOLD_BAR_1 = 295,
        SMALL_GOLD_BAR_2 = 343,
        SMALL_GOLD_BAR_3 = 391,
        SMALL_GOLD_BAR_4 = 342,
        SMALL_GOLD_BAR_5 = 390,
        BIG_GOLD_BAR_1   = 389,
        BIG_GOLD_BAR_2   = 388,
        BIG_GOLD_BAR_3   = 439,
        DOOR_1           = 3004,
        DOOR_2           = 3035
    };

public:
    // Create a new tile.
    explicit Tile();

public:
    // Get the foreground id.
    int fg() const;

    // Set the foreground id.
    void set_fg(int fg);

    // Get the background_1 id.
    int bg_1() const;

    // Set the background_1 id.
    void set_bg_1(int bg);

    // Get the background_2 id.
    int bg_2() const;

    // Set the background_2 id.
    void set_bg_2(int bg);

    // Clear the foreground of any items.
    void clear_fg();

    // Return true if background_2 layer exists.
    bool has_bg_2() const;

    // Return true if foreground layer exists.
    bool has_fg() const;

    // Set the id for a layer.
    void set_layer(Layer layer, int value);

private:
    int m_layers[3];
};
