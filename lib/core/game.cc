// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "core/game.h"

#include <SDL_image.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include <filesystem>
#include <fstream>
#include <ostream>
#include <sstream>

#include "core/common.h"
#include "core/logger.h"

Game::Game()
  : map(*this, m_player)
  , dialogue(*this)
  , emitter(*this)
  , m_frame_rate(60)
  , m_fps_delta(1000 / 60)
  , m_running(false)
  , m_up_cursor(nullptr)
  , m_down_cursor(nullptr)
  , m_cursor_visible(true)
  , m_player(Entity::PLAYER_TYPE, *this)
  , m_transition(false)
  , m_transition_alpha(255)
  , m_menu_visible(true)
  , m_profile_visible(false)
{
    m_health_clip  = { 0, 0, 8, 12 };
    m_mana_clip    = { 0, 12, 8, 12 };
    m_stamina_clip = { 0, 24, 8, 12 };

    for (int i = 0; i < ICON_COUNT; ++i) {
        m_icon_clips[i] = { (i % ICON_SPRITESHEET_COL_COUNT) * ICON_SPRITE_SIZE,
                            (int)(i / ICON_SPRITESHEET_COL_COUNT) * ICON_SPRITE_SIZE, ICON_SPRITE_SIZE,
                            ICON_SPRITE_SIZE };
    }
}

bool Game::initialize()
{
    if (storage.exists()) {
        if (!storage.load()) {
            LOG_ERROR << "Failed to load storage" << std::endl;
            return false;
        }
    } else {
        storage.AddMember("level", 0, storage.GetAllocator());
        storage.AddMember("mute_audio", true, storage.GetAllocator());
        storage.AddMember("menu_visible", true, storage.GetAllocator());
    }

    // Check if audio is muted.
    audio.set_enabled(storage["mute_audio"].GetBool());

    // Check if menu should be visible.
    m_menu_visible = storage["menu_visible"].GetBool();

    if (audio.enabled()) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            LOG_ERROR << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }
    } else {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            LOG_ERROR << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    if (IMG_Init(IMG_INIT_PNG) == 0) {
        LOG_ERROR << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        LOG_ERROR << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!audio.enabled()) {
        return true;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        LOG_ERROR << "Failed to initialize audio device: " << Mix_GetError() << std::endl;
        return false;
    }

    // This will be called when the program terminates.
    std::atexit([]() {
        // Unload SDL_Mix...
        Mix_Quit();

        // Unload SDL_TTF...
        TTF_Quit();

        // Unload SDL_IMG...
        IMG_Quit();

        // Unload SDL...
        SDL_Quit();
    });

    return true;
}

void Game::set_frame_rate(int frame_rate)
{
    if (m_frame_rate == frame_rate) {
        return;
    }

    m_frame_rate = frame_rate;
    m_fps_delta  = 1000 / m_frame_rate;
}

int Game::frame_rate() const
{
    return m_frame_rate;
}

void Game::set_app_name(const char* name)
{
    m_app_name = name;
}

const char* Game::app_name() const
{
    return m_app_name;
}

Texture* Game::get_entity_texture(Entity::Sprite sprite)
{
    if (m_entity_textures.contains(sprite)) {
        return &m_entity_textures[sprite];
    }

    const char* path_to_sheet;

    switch (sprite) {
    case Entity::SPRITE_PLAYER:
        path_to_sheet = RESOLVE_RESOURCE("images/characters/sprites/player-4.png");
        break;
    case Entity::SPRITE_BOAR_MAN:
        path_to_sheet = RESOLVE_RESOURCE("images/characters/sprites/bull-guardian.png");
        break;
    case Entity::SPRITE_BOAR_MAN_GUARDIAN:
        path_to_sheet = RESOLVE_RESOURCE("images/characters/sprites/player-2.png");
        break;
    case Entity::SPRITE_BOAR_MAN_BOSS:
        path_to_sheet = RESOLVE_RESOURCE("images/characters/sprites/player-5.png");
        break;
    }

    m_entity_textures[sprite] = Texture();

    if (!m_entity_textures[sprite].load(window.renderer(), path_to_sheet)) {
        LOG_ERROR << "Failed to load entity texture" << std::endl;
        return nullptr;
    }

    return &m_entity_textures[sprite];
}

int Game::start()
{
    m_running = true;

    // Load our app font.
    m_font = TTF_OpenFont(RESOLVE_RESOURCE("fonts/eskargot.ttf"), 16);

    if (m_font == nullptr) {
        LOG_ERROR << "Failed to load application font: " << TTF_GetError() << std::endl;
        return false;
    }

    if (!window.create(m_app_name)) {
        LOG_ERROR << "Failed to create window" << std::endl;
        return EXIT_FAILURE;
    }

    if (audio.enabled()) {
        audio.play_sound_track(Audio::SoundTrack::DARK_BLUE);
    }

    if (!map.initialize()) {
        LOG_ERROR << "Failed to initialized map" << std::endl;
        return false;
    }

    if (!emitter.initialize()) {
        LOG_ERROR << "Failed to initialize emitter" << std::endl;
        return false;
    }

    if (!dialogue.initialize()) {
        LOG_ERROR << "Failed to initialized dialogue" << std::endl;
        return false;
    }

    if (!map.load_level(storage["level"].GetInt())) {
        LOG_ERROR << "Failed to load map" << std::endl;
        return false;
    }

    if (!m_menu_background_texture.load(window.renderer(), RESOLVE_RESOURCE("images/interfaces/background-2.png"))) {
        LOG_ERROR << "Failed to load menu background texture" << std::endl;
        return false;
    }

    if (m_menu_visible) {
        SDL_ShowCursor(SDL_DISABLE);
        m_cursor_visible = false;
    }

    if (!m_profile_texture.load(window.renderer(), RESOLVE_RESOURCE("images/interfaces/profile.png"))) {
        LOG_ERROR << "Failed to load profile texture" << std::endl;
        return false;
    }

    if (!m_slots_texture.load(window.renderer(), RESOLVE_RESOURCE("images/interfaces/slots.png"))) {
        LOG_ERROR << "Failed to load slots texture" << std::endl;
        return false;
    }

    if (!m_icons_texture.load(window.renderer(), RESOLVE_RESOURCE("images/interfaces/icons.png"))) {
        LOG_ERROR << "Failed to load icons texture" << std::endl;
        return false;
    }

    if (!m_fill_bar_texture.load(window.renderer(), RESOLVE_RESOURCE("images/interfaces/fill-bar.png"))) {
        LOG_ERROR << "Failed to load fill bar texture" << std::endl;
        return false;
    }

    if (!m_fill_level_texture.load(window.renderer(), RESOLVE_RESOURCE("images/interfaces/fill-level.png"))) {
        LOG_ERROR << "Failed to load fill level texture" << std::endl;
        return false;
    }

    m_down_cursor = create_cursor(RESOLVE_RESOURCE("images/interfaces/cursor-2.png"));
    if (m_down_cursor == nullptr) {
        LOG_ERROR << "Failed to load down cursor" << std::endl;
        return false;
    }

    m_up_cursor = create_cursor(RESOLVE_RESOURCE("images/interfaces/cursor.png"));
    if (m_up_cursor == nullptr) {
        LOG_ERROR << "Failed to load up cursor" << std::endl;
        return false;
    }

    SDL_SetCursor(m_up_cursor);

    // Start game loop...
    while (m_running) {
        tick();
    }

    return EXIT_SUCCESS;
}

TTF_Font* Game::font() const
{
    return m_font;
}

void Game::tick()
{
    int start_tick = SDL_GetTicks();

    if (m_transition) {
        if (m_transition_alpha == 0) {
            m_transition       = false;
            m_transition_alpha = 255;
        } else {
            m_transition_alpha--;
        }

        const SDL_Rect rect = { 0, 0, window.width(), window.height() };

        SDL_SetRenderDrawColor(window.renderer(), 0, 0, 0, m_transition_alpha);
        SDL_SetRenderDrawBlendMode(window.renderer(), SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(window.renderer(), &rect);
        SDL_RenderPresent(window.renderer());
    }

    if (m_menu_visible) {
        dialogue.pause();

        SDL_RenderClear(window.renderer());
        m_menu_background_texture.render(0, 0);
        SDL_RenderPresent(window.renderer());

        while (SDL_WaitEventTimeout(&m_event, 100)) {
            if (m_event.type == SDL_MOUSEBUTTONUP) {
                SDL_ShowCursor(SDL_ENABLE);

                m_cursor_visible = true;
                m_menu_visible   = false;

                dialogue.resume();

                return;
            }

            if (m_event.type == SDL_QUIT) {
                return quit();
            }
        }

        return;
    }

    while (SDL_PollEvent(&m_event) != 0) {
        switch (m_event.type) {
        case SDL_MOUSEBUTTONUP:
            handle_mouse_button_up();
            break;
        case SDL_MOUSEBUTTONDOWN:
            handle_mouse_button_down();
            break;
        case SDL_MOUSEMOTION:
            handle_mouse_motion();
            break;
        case SDL_KEYDOWN:
            handle_key_down();
            break;
        case SDL_KEYUP:
            handle_key_up();
            break;
        case SDL_QUIT:
            return quit();
        default:
            break;
        }
    }

    // Clear the current rendering target.
    SDL_RenderClear(window.renderer());

    // Update player logic.
    for (auto& entity : map.entities()) {
        entity->check_collision();
        entity->update(m_physics_timer.ticks());

        // if (!entity->is_player()) {
        //     if (entity->is_near_entity(&m_player)) {
        //         entity->attack(&m_player);
        //     } else {
        //         if (entity->attacking()) {
        //             entity->stop_attacking();
        //         }
        //     }
        // }
    }

    // Update the particle effects.
    emitter.update(m_physics_timer.ticks());

    // Render the map layer.
    map.render();

    // Restart physics timer.
    m_physics_timer.start();

    // Fill bar...
    m_fill_bar_texture.render(8, 8);

    // Render profile button.
    m_icons_texture.render(24, 22, &m_icon_clips[236]);

    // Slots...
    m_slots_texture.render((window.width() / 2) - (462 / 2), window.height() - 50);

    if (auto player_skills = m_player.skills(); !player_skills.empty()) {
        for (size_t i = 0; i < player_skills.size(); ++i) {
            auto skill = player_skills[i];

            if (skill.mana_required() > m_player.mana()) {
                m_icons_texture.set_alpha(100);
            } else {
                m_icons_texture.set_alpha(255);
            }

            m_icons_texture.render((window.width() / 2) - (420 / 2) + (i * 42) + (i + 2), window.height() - 45,
                                   &m_icon_clips[skill.type()]);
        }
    }

    for (int i = 0; i < m_player.health_bars(); ++i) {
        m_fill_level_texture.render(93 + i * 9, 14, &m_health_clip);
    }

    for (int i = 0; i < m_player.mana_bars(); ++i) {
        m_fill_level_texture.render(93 + i * 9, 34, &m_mana_clip);
    }

    for (int i = 0; i < m_player.stamina_bars(); ++i) {
        m_fill_level_texture.render(93 + i * 9, 55, &m_stamina_clip);
    }

    // Render the dialogue at the bottom of the screen.
    dialogue.render();

    // Render the minimap in top right corner.
    map.render_minimap();

    if (m_profile_visible) {
        const SDL_Rect rect = { 0, 0, window.width(), window.height() };

        SDL_SetRenderDrawColor(window.renderer(), 0, 0, 0, 255 / 2);
        SDL_SetRenderDrawBlendMode(window.renderer(), SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(window.renderer(), &rect);

        m_profile_texture.render((window.width() / 2) - (400 / 2), (window.height() / 2) - (250 / 2));

        const auto lines = {
            "???",
        };

        int i = 0;

        for (auto line : lines) {
            m_profile_text_texture.load_from_text(window.renderer(), line, m_font, { 255, 255, 255, 255 });
            m_profile_text_texture.render((window.width() / 2) - (400 / 2) + 16,
                                          (window.height() / 2) - (250 / 2) + 36 + (16 * (i)));
            ++i;
        }

        SDL_RenderPresent(window.renderer());

        while (SDL_WaitEventTimeout(&m_event, 100)) {
            if (m_event.type == SDL_MOUSEBUTTONUP) {
                m_profile_visible = false;
                return;
            } else if (m_event.type == SDL_KEYUP) {
                switch (m_event.key.keysym.sym) {
                default:
                    m_profile_visible = false;
                    break;
                }

                if (m_event.type == SDL_QUIT) {
                    return quit();
                }
            }

            return;
        }
    }

    // Reset the render color.
    SDL_SetRenderDrawColor(window.renderer(), 0, 0, 0, 255);

    // Update the screen.
    SDL_RenderPresent(window.renderer());

    // To avoid using 100% CPU...
    if (int delta = SDL_GetTicks() - start_tick; delta < m_fps_delta) {
        SDL_Delay(m_fps_delta - delta);
    }
}

void Game::handle_mouse_button_up()
{
    SDL_SetCursor(m_up_cursor);

    m_player.walk_to_position(map.camera_offset_x(m_event.button.x / WINDOW_SCALE),
                              map.camera_offset_y(m_event.button.y / WINDOW_SCALE));
}

void Game::handle_mouse_button_down()
{
    SDL_SetCursor(m_down_cursor);
}

void Game::handle_mouse_motion()
{
    if (!m_cursor_visible) {
        SDL_ShowCursor(SDL_ENABLE);
        m_cursor_visible = true;
    }
}

void Game::handle_key_down()
{
    if (dialogue.is_exchange_playing()) {
        m_player.stop_walking();
        return;
    }

    m_key_state = SDL_GetKeyboardState(NULL);

    if (m_key_state[SDL_SCANCODE_DOWN] || m_key_state[SDL_SCANCODE_S]) {
        if (!m_key_state[SDL_SCANCODE_UP] || m_key_state[SDL_SCANCODE_W]) {
            m_player.walk_in_direction(Entity::Direction::DOWN);
        }
    } else {
        if (m_key_state[SDL_SCANCODE_UP] || m_key_state[SDL_SCANCODE_W]) {
            m_player.walk_in_direction(Entity::Direction::UP);
        }
    }

    if (m_key_state[SDL_SCANCODE_RIGHT] || m_key_state[SDL_SCANCODE_D]) {
        if (!m_key_state[SDL_SCANCODE_LEFT] || m_key_state[SDL_SCANCODE_A]) {
            m_player.walk_in_direction(Entity::Direction::RIGHT);
        }
    } else {
        if (m_key_state[SDL_SCANCODE_LEFT] || m_key_state[SDL_SCANCODE_A]) {
            m_player.walk_in_direction(Entity::Direction::LEFT);
        }
    }

    if (m_cursor_visible) {
        SDL_ShowCursor(SDL_DISABLE);
        m_cursor_visible = false;
    }
}

void Game::handle_key_up()
{
    switch (m_event.key.keysym.sym) {
    case SDLK_1:
        m_player.use_skill(0);
        break;
    case SDLK_2:
        m_player.use_skill(1);
        break;
    case SDLK_3:
        m_player.use_skill(2);
        break;
    case SDLK_4:
        m_player.use_skill(3);
        break;
    case SDLK_5:
        m_player.use_skill(4);
        break;
    case SDLK_6:
        m_player.use_skill(5);
        break;
    case SDLK_7:
        m_player.use_skill(6);
        break;
    case SDLK_8:
        m_player.use_skill(7);
        break;
    case SDLK_9:
        m_player.use_skill(8);
        break;
    case SDLK_0:
        m_player.use_skill(10);
        break;
    case SDLK_p:
        m_menu_visible = true;
        break;
    case SDLK_i:
        m_profile_visible = true;
        break;
    case SDLK_f:
        m_player.auto_attack();
        break;
    case SDLK_m:
        audio.toggle_mute();
        break;
    case SDLK_SPACE:
        m_player.jump();
        break;
    case SDLK_UP:
    case SDLK_w:
        m_player.stop_walking_in_direction(Entity::Direction::UP);
        break;
    case SDLK_DOWN:
    case SDLK_s:
        m_player.stop_walking_in_direction(Entity::Direction::DOWN);
        break;
    case SDLK_LEFT:
    case SDLK_a:
        m_player.stop_walking_in_direction(Entity::Direction::LEFT);
        break;
    case SDLK_RIGHT:
    case SDLK_d:
        m_player.stop_walking_in_direction(Entity::Direction::RIGHT);
        break;
    default:
        break;
    }
}

SDL_Cursor* Game::create_cursor(const char* file_name)
{
    auto surface = IMG_Load(file_name);

    if (SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0xFF, 0xFF)) < 0) {
        return nullptr;
    }

    if (!surface) {
        return nullptr;
    }

    // Use a custom 64x64 cursor.
    auto cursor = SDL_CreateColorCursor(surface, 0, 0);

    if (!cursor) {
        return nullptr;
    }

    return cursor;
}

void Game::quit()
{
    LOG_DEBUG << "Quitting application..." << std::endl;

    if (audio.muted() != storage["mute_audio"].GetBool()) {
        storage["mute_audio"].SetBool(audio.muted());
    }

    if (map.level() != storage["level"].GetInt()) {
        storage["level"].SetInt(map.level());
    }

    // Save settings to storage.
    if (!storage.save()) {
        LOG_ERROR << "Failed to save storage file" << std::endl;
    }

    if (m_font != nullptr) {
        TTF_CloseFont(m_font);
    }

    if (m_down_cursor != nullptr) {
        SDL_FreeCursor(m_down_cursor);
    }

    if (m_up_cursor != nullptr) {
        SDL_FreeCursor(m_up_cursor);
    }

    m_running = false;
}

Game::~Game()
{
    if (m_running) {
        quit();
    }
}
