// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <string>
#include <vector>

#include "characters/entity.h"
#include "core/common.h"
#include "graphics/texture.h"
#include "maps/search.h"
#include "maps/tile.h"

class Game;

// Map represents the tilemap for the game. It lays out all of the entities
// and background/foreground tiles.
class Map
{
public:
    // Create a new map for the game.
    explicit Map(Game& game, Entity& player);

    // Map destructor.
    ~Map();

public:
    // Initialize the map. Only needs to be called once.
    bool initialize();

    // Load the map at the current level.
    bool load_level(int level);

    // Return the current map level.
    int level() const;

    // Return the tile at the given position.
    Tile& at(int row, int col);

    // X camera offset.
    int camera_offset_x(int x) const;

    // Y camera offset.
    int camera_offset_y(int y) const;

    // Return all entities on the map.
    std::vector<Entity*>& entities();

    // Render the minimap which is a scaled down version of the map with only entities visible.
    void render_minimap();

    // Go to the next map level.
    void go_to_next_level();

    // Go to the previous map level.
    void go_to_prev_level();

    // Render the map and entities.
    void render();

private:
    int m_level;

    Game&   m_game;
    Entity& m_player;

    Texture m_texture;
    Texture m_minimap_texture;
    Texture m_flashlight_texture;

    SDL_Rect             m_camera;
    std::vector<Entity*> m_entities;

    SDL_Rect m_clips[MAP_TILE_SPRITESHEET_SIZE];
    Tile     m_tiles[MAP_TILE_COL_COUNT * MAP_TILE_ROW_COUNT];

    SearchGraph m_search_graph;
};
