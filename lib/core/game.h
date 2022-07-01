// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>

#include <unordered_map>

#include "audio/audio.h"
#include "characters/entity.h"
#include "core/common.h"
#include "core/dialogue.h"
#include "core/timer.h"
#include "graphics/texture.h"
#include "graphics/window.h"
#include "maps/map.h"
#include "particles/emitter.h"
#include "storage/storage.h"

class Game
{
public:
    explicit Game();
    ~Game();

public:
    void        set_frame_rate(int frame_rate);
    int         frame_rate() const;
    const char* app_name() const;
    void        set_app_name(const char* name);
    bool        initialize();
    bool        running() const;
    Texture*    get_entity_texture(Entity::Sprite sprite);
    int         start();
    void        quit();
    TTF_Font*   font() const;
    SDL_Cursor* create_cursor(const char* file_name);

public:
    // The window controller.
    Window window;

    // The audio controller.
    Audio audio;

    // The storage controller.
    Storage storage;

    // The map controller.
    Map map;

    // The dialogue controller.
    Dialogue dialogue;

    // The particles controller.
    Emitter emitter;

private:
    void tick();

    // Handle key events.
    void handle_key_down();
    void handle_key_up();

    // Handle mouse events.
    void handle_mouse_button_up();
    void handle_mouse_button_down();
    void handle_mouse_motion();

private:
    int m_frame_rate;
    int m_fps_delta;

    bool        m_running;
    const char* m_app_name;

    SDL_Event   m_event;
    TTF_Font*   m_font;
    SDL_Cursor* m_up_cursor;
    SDL_Cursor* m_down_cursor;

    Texture m_slots_texture;
    Texture m_fill_level_texture;
    Texture m_fill_bar_texture;
    Texture m_frame_rate_texture;
    Texture m_icons_texture;
    Texture m_menu_background_texture;
    Texture m_profile_texture;
    Texture m_profile_text_texture;

    std::unordered_map<Entity::Sprite, Texture> m_entity_textures;

    SDL_Rect m_health_clip;
    SDL_Rect m_mana_clip;
    SDL_Rect m_stamina_clip;

    SDL_Rect m_icon_clips[ICON_COUNT];
    bool     m_cursor_visible;

    Timer m_physics_timer;

    Entity m_player;

    bool m_transition;
    int  m_transition_alpha;

    const uint8_t* m_key_state;

    bool m_menu_visible;
    bool m_profile_visible;
};
