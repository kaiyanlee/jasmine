// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <cstdint>

#ifndef NDEBUG
#define DEBUG_MODE
#endif

// Window scale.
const static auto WINDOW_SCALE = 4;

// Map tile dimensions.
static const auto MAP_TILE_ROW_COUNT = 100;
static const auto MAP_TILE_COL_COUNT = 100;

// Map tile size.
static const auto MAP_TILE_SIZE = 32;

// Physics constants.
static const float GRAVITY = 9.81;

// Map dimensions.
static const auto MAP_WIDTH  = MAP_TILE_COL_COUNT * MAP_TILE_SIZE;
static const auto MAP_HEIGHT = MAP_TILE_ROW_COUNT * MAP_TILE_SIZE;

// Map spritesheet dimensions.
static const auto MAP_TILE_SPRITESHEET_ROW_COUNT = 91;
static const auto MAP_TILE_SPRITESHEET_COL_COUNT = 48;
static const auto MAP_TILE_SPRITESHEET_SIZE      = MAP_TILE_SPRITESHEET_COL_COUNT * MAP_TILE_SPRITESHEET_ROW_COUNT;

// The number of icons in the icon spritesheet.
static const auto ICON_SPRITESHEET_ROW_COUNT = 16;
static const auto ICON_SPRITESHEET_COL_COUNT = 31;
static const auto ICON_COUNT                 = ICON_SPRITESHEET_ROW_COUNT * ICON_SPRITESHEET_COL_COUNT;

// The size of each icon in the icon spritesheet.
static const auto ICON_SPRITE_SIZE = 30;

// Character dimensions.
static const int CHARACTER_HEIGHT = 64;
static const int CHARACTER_WIDTH  = 64;

// Number of bars (health, mana, and stamina).
static const auto BAR_COUNT = 14;

// Number of available slots.
static const auto SLOTS_AVAILABLE = 14;

// Tiled tile flags.
const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
const unsigned FLIPPED_VERTICALLY_FLAG   = 0x40000000;
const unsigned FLIPPED_DIAGONALLY_FLAG   = 0x20000000;

const static uint8_t SOUND_TRACK_COUNT = 1;
const static uint8_t SOUND_COUNT       = 51;

// Resolve a file in the data folder.
#define RESOLVE_DATA(FILE_NAME) DATA_FOLDER "/" FILE_NAME

// Resolve a file in the resource/media folder.
#define RESOLVE_RESOURCE(FILE_NAME) RESOURCE_FOLDER "/" FILE_NAME

// Resolve a file in the configuration folder.
#define RESOLVE_CONFIG(FILE_NAME) CONFIG_FOLDER "/" FILE_NAME
