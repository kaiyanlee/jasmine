// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "maps/tile.h"

Tile::Tile()
  : m_layers()
{
    m_layers[FOREGROUND]   = -1;
    m_layers[BACKGROUND_1] = -1;
    m_layers[BACKGROUND_2] = -1;
}

int Tile::fg() const
{
    return m_layers[FOREGROUND];
}

void Tile::set_fg(int fg)
{
    m_layers[FOREGROUND] = fg;
}

int Tile::bg_1() const
{
    return m_layers[BACKGROUND_1];
}

int Tile::bg_2() const
{
    return m_layers[BACKGROUND_2];
}

void Tile::clear_fg()
{
    m_layers[FOREGROUND] = -1;
}

void Tile::set_bg_1(int bg)
{
    m_layers[BACKGROUND_1] = bg;
}

void Tile::set_bg_2(int bg)
{
    m_layers[BACKGROUND_2] = bg;
}

bool Tile::has_bg_2() const
{
    return bg_2() >= 0;
}

bool Tile::has_fg() const
{
    return fg() >= 0;
}

void Tile::set_layer(Layer layer, int value)
{
    m_layers[layer] = value;
}
