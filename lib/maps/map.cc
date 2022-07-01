// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "maps/map.h"

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/stringbuffer.h>
#include <zlib.h>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>

#include "core/common.h"
#include "core/game.h"
#include "core/logger.h"

// The minimap scale (higher = more coverage).
static const auto MINIMAP_SCALE = 20;

// The minimap dot size.
static const auto MINIMAP_DOT_SIZE = 2;

// Decode a Base64 encoded string.
//
// Reference: https://stackoverflow.com/questions/180947/base64-decode-snippet-in-c
std::string base64_decode(const std::string_view data)
{
    LOG_DEBUG << "Base64 decode: " << data.length() << std::endl;

    const static uint8_t lookup[] = { 62,  255, 62,  255, 63,  52,  53, 54, 55, 56, 57, 58, 59, 60, 61, 255,
                                      255, 0,   255, 255, 255, 255, 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
                                      10,  11,  12,  13,  14,  15,  16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
                                      255, 255, 255, 255, 63,  255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
                                      36,  37,  38,  39,  40,  41,  42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

    static_assert(sizeof(lookup) == 'z' - '+' + 1);

    std::string result;

    int v  = 0;
    int vb = -8;

    for (uint8_t c : data) {
        if (c < '+' || c > 'z') {
            break;
        }

        if (c -= '+'; lookup[c] >= 64) {
            break;
        }

        v = (v << 6) + lookup[c];
        vb += 6;

        if (vb >= 0) {
            result.push_back(char((v >> vb) & 0xFF));
            vb -= 8;
        }
    }

    return result;
}

// MIT License. Copyright (c) Mark Winter.
//
// Reference: https://github.com/markwinter/tmxjson/blob/master/src/Parsers.cpp
bool zlib_inflate(const char* source, std::size_t inSize, std::vector<unsigned char>& dest, std::size_t expectedSize)
{
    LOG_DEBUG << "Decompressing: " << inSize << std::endl;

    int current_size = static_cast<int>(expectedSize);

    std::vector<unsigned char> byte_array(expectedSize / sizeof(unsigned char));

    z_stream stream;
    stream.zalloc    = Z_NULL;
    stream.zfree     = Z_NULL;
    stream.opaque    = Z_NULL;
    stream.next_in   = (Byte*)(source);
    stream.avail_in  = static_cast<unsigned int>(inSize);
    stream.next_out  = (Byte*)(byte_array.data());
    stream.avail_out = static_cast<unsigned int>(expectedSize);

    if (inflateInit(&stream) != Z_OK) {
        LOG_ERROR << "Failed to initialize zlib" << std::endl;
        return false;
    }

    int result = Z_OK;
    while (result != Z_STREAM_END) {
        result = inflate(&stream, Z_SYNC_FLUSH);

        switch (result) {
        case Z_NEED_DICT:
        case Z_ERRNO:
        case Z_STREAM_ERROR:
        case Z_DATA_ERROR:
        case Z_MEM_ERROR:
        case Z_BUF_ERROR:
        case Z_VERSION_ERROR:
            if (auto s = inflateEnd(&stream); s != Z_OK) {
                LOG_ERROR << "ZLib inflateEnd() failed: " << std::to_string(s) << std::endl;
                return false;
            }

            LOG_ERROR << "ZLib inflate() failed: " << std::to_string(result) << std::endl;

            return false;
        }

        if (result != Z_STREAM_END && !stream.avail_out) {
            // Run out of space, so lets increase our space and continue
            int old_size = current_size;
            current_size *= 2;

            std::vector<unsigned char> new_data(current_size / sizeof(unsigned char));
            std::memcpy(new_data.data(), byte_array.data(), old_size);
            byte_array = std::move(new_data);

            stream.next_out  = (Byte*)(byte_array.data() + old_size);
            stream.avail_out = old_size;
        }
    }

    const int out_size = current_size - stream.avail_out;

    if (auto s = inflateEnd(&stream); s != Z_OK) {
        LOG_ERROR << "ZLib inflateEnd() failed: " << std::to_string(s) << std::endl;
        return false;
    }

    std::vector<unsigned char> new_array(out_size / sizeof(unsigned char));
    std::memcpy(new_array.data(), byte_array.data(), out_size);
    byte_array = std::move(new_array);

    dest.insert(dest.begin(), byte_array.begin(), byte_array.end());

    LOG_DEBUG << "Decompression successful: " << out_size << std::endl;

    return true;
}

Map::Map(Game& game, Entity& player)
  : m_game(game)
  , m_player(player)
  , m_search_graph(m_tiles)
{
    for (int i = 0; i < MAP_TILE_SPRITESHEET_ROW_COUNT; ++i) {
        for (int j = 0; j < MAP_TILE_SPRITESHEET_COL_COUNT; ++j) {
            m_clips[j + (i * MAP_TILE_SPRITESHEET_COL_COUNT)] = { j * MAP_TILE_SIZE, i * MAP_TILE_SIZE, MAP_TILE_SIZE,
                                                                  MAP_TILE_SIZE };
        }
    }
}

bool Map::initialize()
{
    if (!m_texture.load(m_game.window.renderer(), RESOLVE_RESOURCE("images/landscapes/sprites.png"))) {
        LOG_ERROR << "Failed to load texture" << std::endl;
        return false;
    }

    if (!m_minimap_texture.load(m_game.window.renderer(), RESOLVE_RESOURCE("images/interfaces/minimap.png"))) {
        LOG_ERROR << "Failed to load slots texture" << std::endl;
        return false;
    }

    // if (!m_flashlight_texture.load(m_game.window.renderer(), RESOLVE_RESOURCE("images/interfaces/flashlight.png"))) {
    //     LOG_ERROR << "Failed to load flashlight texture" << std::endl;
    //     return false;
    // }

    m_camera.w = m_game.window.width();
    m_camera.h = m_game.window.height();

    // Push the player to the front.
    m_entities.insert(m_entities.begin(), &m_player);

    return true;
}

bool Map::load_level(int level)
{
    LOG_DEBUG << "Loading level: " << level << std::endl;

    switch (level) {
    case 0:
        m_game.dialogue.play_notice(Dialogue::Notice::ENTER_LEVEL_0);
        break;
    case 1:
        m_game.dialogue.play_notice(Dialogue::Notice::ENTER_LEVEL_1);
        break;
    case 2:
        m_game.dialogue.play_notice(Dialogue::Notice::ENTER_LEVEL_2);
        break;
    case 3:
        m_game.dialogue.play_notice(Dialogue::Notice::ENTER_LEVEL_3);
        break;
    case 4:
        m_game.dialogue.play_notice(Dialogue::Notice::ENTER_LEVEL_4);
        break;
    case 5:
        m_game.dialogue.play_notice(Dialogue::Notice::ENTER_LEVEL_5);
        break;
    default:
        break;
    }

    // The map level data is stored in a json file.
    std::ifstream file(std::string(RESOLVE_DATA("maps/map.")) + std::to_string(level) + std::string(".json"));

    if (!file.is_open()) {
        LOG_ERROR << "Failed to load map for level: " << level << std::endl;
        return false;
    }

    rapidjson::IStreamWrapper isw(file);
    rapidjson::Document       doc;

    LOG_DEBUG << "Parsing JSON document" << std::endl;

    doc.ParseStream(isw);

    for (const auto& layer : doc["layers"].GetArray()) {
        LOG_DEBUG << "Parsing layer\n";

        if (layer.HasMember("data")) {
            LOG_DEBUG << "Parsing data layer...\n";

            const auto& object = layer["data"];
            const auto& name   = layer["name"];

            std::vector<unsigned char> dest;

            LOG_DEBUG << "Decoding layer tile data...\n";

            // Decode from base64...
            auto data = base64_decode(object.GetString());

            // Decompress data...
            if (!zlib_inflate(data.c_str(), data.length() * sizeof(unsigned char), dest, data.length() * 5)) {
                return false;
            }

            int      i, j;
            int      buffer_index = 0;
            uint32_t gid;

            Tile::Layer layer;

            if (name == "bg_1") {
                layer = Tile::BACKGROUND_1;
            } else if (name == "fg") {
                layer = Tile::FOREGROUND;
            } else {
                layer = Tile::BACKGROUND_2;
            }

            LOG_DEBUG << "Populating map with new tile data...\n";

            for (i = 0; i < MAP_TILE_ROW_COUNT; ++i) {
                for (j = 0; j < MAP_TILE_COL_COUNT; ++j) {
                    // Get the tile global id.
                    gid = dest[buffer_index] | dest[buffer_index + 1] << 8 | dest[buffer_index + 2] << 16
                          | dest[buffer_index + 3] << 24;

                    // Remove flags...
                    gid &= ~(FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG);

                    // Increment the buffer index.
                    buffer_index += 4;

                    // Tiled stores ids as unsigned integers...
                    if (int id = gid - 1; id >= 0) {
                        if (layer == Tile::FOREGROUND && id != 1111) {
                            LOG_DEBUG << i << " " << j << " " << id << "\n";
                        }

                        m_tiles[i + (j * MAP_TILE_COL_COUNT)].set_layer(layer, id);
                    } else if (id < -1) {
                        LOG_ERROR << "Error parsing map: " << i << " " << j << " " << gid << " " << int(gid - 1)
                                  << "\n";
                    } else {
                        m_tiles[i + (j * MAP_TILE_COL_COUNT)].set_layer(layer, -1);
                    }
                }
            }
        } else {
            LOG_DEBUG << "Parsing object layer...\n";

            auto iter = m_entities.begin();

            while (iter != m_entities.end()) {
                if (auto entity = *iter; !entity->is_player()) {
                    delete entity;
                    m_entities.erase(iter);
                } else {
                    ++iter;
                }
            }

            for (const auto& object : layer["objects"].GetArray()) {
                const auto name = object["name"].GetString();

                if (strcmp(name, "Enemy") == 0) {
                    auto enemy = new Entity(Entity::ENEMY_TYPE, m_game);
                    enemy->set_position(object["x"].GetInt(), object["y"].GetInt() - CHARACTER_HEIGHT);

                    auto type = std::stoi(std::string(object["type"].GetString()));

                    if (!enemy->set_sprite(static_cast<Entity::Sprite>(type))) {
                        return false;
                    }

                    // Push the enemy to the back.
                    m_entities.insert(m_entities.end(), enemy);
                } else if (strcmp(name, "Player") == 0) {
                    m_player.set_position(object["x"].GetInt(), object["y"].GetInt() - CHARACTER_HEIGHT);

                    auto type = std::stoi(std::string(object["type"].GetString()));

                    if (!m_player.set_sprite(static_cast<Entity::Sprite>(type))) {
                        return false;
                    }
                }
            }
        }
    }

    if (m_level != level) {
        m_level = level;
    }

    file.close();

    LOG_DEBUG << "Finished loading map" << std::endl;

    return true;
}

Tile& Map::at(int row, int col)
{
    return m_tiles[row + (col * MAP_TILE_ROW_COUNT)];
}

int Map::level() const
{
    return m_level;
}

void Map::go_to_next_level()
{
    if (!load_level(m_level + 1)) {
        LOG_ERROR << "Failed to enter next level" << std::endl;
    }
}

void Map::go_to_prev_level()
{
    if (!load_level(m_level - 1)) {
        LOG_ERROR << "Failed to enter previous level" << std::endl;
    }
}

std::vector<Entity*>& Map::entities()
{
    return m_entities;
}

int Map::camera_offset_x(int x) const
{
    return x + m_camera.x;
}

int Map::camera_offset_y(int y) const
{
    return y + m_camera.y;
}

void Map::render()
{
    m_camera.x = (m_player.pos_x() + CHARACTER_WIDTH / 2) - m_game.window.width() / 2;
    m_camera.y = (m_player.pos_y() + CHARACTER_HEIGHT / 2) - m_game.window.height() / 2;

    if (m_camera.x < 0) {
        m_camera.x = 0;
    }

    if (m_camera.y < 0) {
        m_camera.y = 0;
    }

    if (m_camera.x >= MAP_WIDTH - m_camera.w) {
        m_camera.x = MAP_WIDTH - m_camera.w;
    }

    if (m_camera.y >= MAP_HEIGHT - m_camera.h) {
        m_camera.y = MAP_HEIGHT - m_camera.h;
    }

    Tile* tile;

    int row, col, x, y;

    int min_row = m_camera.y / MAP_TILE_SIZE;
    int max_row = std::min(MAP_TILE_ROW_COUNT, ((m_camera.y + m_camera.h) / MAP_TILE_SIZE) + 1);

    int min_col = m_camera.x / MAP_TILE_SIZE;
    int max_col = std::min(MAP_TILE_COL_COUNT, ((m_camera.x + m_camera.w) / MAP_TILE_SIZE) + 1);

    // Render the background.
    for (row = min_row; row < max_row; ++row) {
        for (col = min_col; col < max_col; ++col) {
            tile = &m_tiles[row + (col * MAP_TILE_ROW_COUNT)];

            x = (col * MAP_TILE_SIZE) - m_camera.x;
            y = (row * MAP_TILE_SIZE) - m_camera.y;

            // Render the background layer 1.
            m_texture.render(x, y, &m_clips[tile->bg_1()]);

            // Render the background layer 2.
            if (tile->has_bg_2()) {
                m_texture.render(x, y, &m_clips[tile->bg_2()]);
            }
        }
    }

    // const SDL_Rect rect = { 0, 0, m_game.window.width(), m_game.window.height() };

    // SDL_SetRenderDrawColor(m_game.window.renderer(), 0, 0, 0, 200);
    // SDL_SetRenderDrawBlendMode(m_game.window.renderer(), SDL_BLENDMODE_BLEND);
    // SDL_RenderFillRect(m_game.window.renderer(), &rect);

    // const auto mode = SDL_ComposeCustomBlendMode(SDL_BLENDFACTOR_DST_ALPHA, SDL_BLENDFACTOR_DST_ALPHA,
    //                                              SDL_BLENDOPERATION_REV_SUBTRACT,
    //                                              SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA,
    //                                              SDL_BLENDFACTOR_ONE_MINUS_DST_ALPHA, SDL_BLENDOPERATION_SUBTRACT);

    // m_flashlight_texture->set_blend_mode(mode);
    // m_flashlight_texture->render(m_player.pos_x() - m_camera.x - 128 + 32, m_player.pos_y() - m_camera.y - 128 + 32);

    // Render the foreground and objects.
    for (row = min_row; row < max_row; ++row) {
        for (col = min_col; col < max_col; ++col) {
            tile = &m_tiles[row + (col * MAP_TILE_ROW_COUNT)];

            // Render the foreground.
            if (tile->has_fg()) {
                x = (col * MAP_TILE_SIZE) - m_camera.x;
                y = (row * MAP_TILE_SIZE) - m_camera.y;

                m_texture.render(x, y, &m_clips[tile->fg()]);
            }
        }
    }

    std::sort(m_entities.begin(), m_entities.end(), [](Entity* a, Entity* b) {
        return a->pos_y() > b->pos_y();
    });

    for (auto iter = m_entities.rbegin(); iter != m_entities.rend(); ++iter) {
        (*iter)->render(m_camera);
    }

    // Render the particle effects.
    m_game.emitter.render(m_camera);
}

void Map::render_minimap()
{
    // Render the minimap frame.
    m_minimap_texture.render(m_game.window.width() - 138, 8);

    int      rx, ry;
    SDL_Rect pr;

    for (auto& entity : m_entities) {
        rx = m_game.window.width() - 90 + (entity->pos_x() - m_camera.x) / MINIMAP_SCALE;
        ry = 70 + (entity->pos_y() - m_camera.y) / MINIMAP_SCALE;
        pr = { rx, ry, MINIMAP_DOT_SIZE, MINIMAP_DOT_SIZE };

        if (rx > m_game.window.width() - 110 && ry > 40 && rx < m_game.window.width() - 50 && ry < 100) {
            if (entity->is_player()) {
                SDL_SetRenderDrawColor(m_game.window.renderer(), 0, 255, 0, 255);
            } else {
                SDL_SetRenderDrawColor(m_game.window.renderer(), 255, 0, 0, 255);
            }

            SDL_RenderFillRect(m_game.window.renderer(), &pr);
        }
    }
}

Map::~Map()
{
    LOG_DEBUG << "Destroying map" << std::endl;

    auto iter = m_entities.begin();

    // Only remove the enemies (the player is not dynamically allocated).
    while (iter != m_entities.end()) {
        if (auto& entity = *iter; !entity->is_player()) {
            delete *iter;
            m_entities.erase(iter);
        } else {
            ++iter;
        }
    }
}
