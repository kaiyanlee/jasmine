// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "maps/search.h"

#include <queue>
#include <unordered_map>

SearchGraph::SearchGraph(Tile (&tiles)[MAP_TILE_COL_COUNT * MAP_TILE_ROW_COUNT])
  : m_map_tiles(tiles)
{
    (void)m_map_tiles;
}

void SearchGraph::find_path(Cell src, Cell dest, std::vector<Cell>& path)
{
    // Reset our directions...
    if (!path.empty()) {
        path.clear();
    }

    // Already at destination, nothing to do...
    if (src.column == dest.column && src.row == dest.row) {
        return;
    }

    std::priority_queue<Node> open_set;
    std::vector<Node>         came_from;

    std::unordered_map<Cell*, int> g_score = { { &src, 0 } };
    std::unordered_map<Cell*, int> f_score = { { &src, calculate_cost(src, dest) } };
}

uint32_t SearchGraph::calculate_cost(Cell src, Cell dest) const
{
    (void)src;
    (void)dest;

    return 0;
}
