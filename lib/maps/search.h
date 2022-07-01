// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <vector>

#include "core/common.h"
#include "maps/tile.h"

// A* algorithm implementation.
//
// TODO: Create implementation (currently a placeholder).
//
// See: https://en.wikipedia.org/wiki/A*_search_algorithm#Applications
class SearchGraph
{
public:
    struct Cell
    {
        int row;
        int column;
    };

    struct Node
    {
        uint32_t score;
        Cell     position;
        uint8_t  prev;
    };

public:
    // Create a new search graph given the tile data.
    explicit SearchGraph(Tile (&tiles)[MAP_TILE_COL_COUNT * MAP_TILE_ROW_COUNT]);

public:
    // Find the shortest path from source to destination.
    void find_path(Cell src, Cell dest, std::vector<Cell>& path);

    // Calculate the cost of moving to destination.
    uint32_t calculate_cost(Cell src, Cell dest) const;

private:
    Tile (&m_map_tiles)[MAP_TILE_COL_COUNT * MAP_TILE_ROW_COUNT];
};
