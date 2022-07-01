// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "audio/audio.h"

#include "core/logger.h"

Audio::Audio()
{
    for (uint8_t i = 0; i < SOUND_COUNT; ++i) {
        m_sounds[i] = nullptr;
    }

    for (uint8_t i = 0; i < SOUND_TRACK_COUNT; ++i) {
        m_sound_tracks[i] = nullptr;
    }
}

void Audio::play_sound(Sound sound, int loops)
{
    if (!m_enabled) {
        return;
    }

    if (m_sounds[sound] == nullptr) {
        switch (sound) {
        case BELT_HANDLE1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/belt-handle1.ogg"));
            break;
        case BELT_HANDLE2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/belt-handle2.ogg"));
            break;
        case BOOK_CLOSE:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-close.ogg"));
            break;
        case BOOK_FLIP1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-flip1.ogg"));
            break;
        case BOOK_FLIP2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-flip2.ogg"));
            break;
        case BOOK_FLIP3:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-flip3.ogg"));
            break;
        case BOOK_OPEN:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-open.ogg"));
            break;
        case BOOK_PLACE1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-place1.ogg"));
            break;
        case BOOK_PLACE2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-place2.ogg"));
            break;
        case BOOK_PLACE3:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/book-place3.ogg"));
            break;
        case CHOP:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/chop.ogg"));
            break;
        case CLOTH1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/cloth1.ogg"));
            break;
        case CLOTH2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/cloth2.ogg"));
            break;
        case CLOTH3:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/cloth3.ogg"));
            break;
        case CLOTH4:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/cloth4.ogg"));
            break;
        case CLOTH_BELT2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/cloth-belt2.ogg"));
            break;
        case CLOTH_BELT:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/cloth-belt.ogg"));
            break;
        case CREAK1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/creak1.ogg"));
            break;
        case CREAK2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/creak2.ogg"));
            break;
        case CREAK3:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/creak3.ogg"));
            break;
        case DOOR_CLOSE_1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/door-close_1.ogg"));
            break;
        case DOOR_CLOSE_2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/door-close_2.ogg"));
            break;
        case DOOR_CLOSE_3:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/door-close_3.ogg"));
            break;
        case DOOR_CLOSE_4:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/door-close_4.ogg"));
            break;
        case DOOR_OPEN_1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/door-open_1.ogg"));
            break;
        case DOOR_OPEN_2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/door-open_2.ogg"));
            break;
        case DRAW_KNIFE1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/draw-knife1.ogg"));
            break;
        case DRAW_KNIFE2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/draw-knife2.ogg"));
            break;
        case DRAW_KNIFE3:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/draw-knife3.ogg"));
            break;
        case DROP_LEATHER:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/drop_leather.ogg"));
            break;
        case FOOTSTEP00:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep00.ogg"));
            break;
        case FOOTSTEP01:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep01.ogg"));
            break;
        case FOOTSTEP02:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep02.ogg"));
            break;
        case FOOTSTEP03:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep03.ogg"));
            break;
        case FOOTSTEP04:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep04.ogg"));
            break;
        case FOOTSTEP05:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep05.ogg"));
            break;
        case FOOTSTEP06:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep06.ogg"));
            break;
        case FOOTSTEP07:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep07.ogg"));
            break;
        case FOOTSTEP08:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep08.ogg"));
            break;
        case FOOTSTEP09:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/footstep09.ogg"));
            break;
        case HANDLE_COINS2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/handle-coins2.ogg"));
            break;
        case HANDLE_COINS:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/handle-coins.ogg"));
            break;
        case HANDLE_SMALL_LEATHER2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/handle-small-leather2.ogg"));
            break;
        case HANDLE_SMALL_LEATHER:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/handle-small-leather.ogg"));
            break;
        case KNIFE_SLICE2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/knife-slice2.ogg"));
            break;
        case KNIFE_SLICE:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/knife-slice.ogg"));
            break;
        case METAL_CLICK:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/metal-click.ogg"));
            break;
        case METAL_LATCH:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/metal-latch.ogg"));
            break;
        case METAL_POT1:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/metal-pot1.ogg"));
            break;
        case METAL_POT2:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/metal-pot2.ogg"));
            break;
        case METAL_POT3:
            m_sounds[sound] = Mix_LoadWAV(RESOLVE_RESOURCE("audio/sounds/metal-pot3.ogg"));
            break;
        }
    }

    Mix_PlayChannelTimed(-1, m_sounds[sound], loops, 5000);
}

void Audio::toggle_mute()
{
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
        m_enabled = true;
    } else {
        Mix_PauseMusic();
        m_enabled = false;
    }
}

bool Audio::muted() const
{
    return Mix_PausedMusic();
}

bool Audio::enabled() const
{
    return m_enabled;
}

void Audio::set_enabled(bool enabled)
{
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;

    if (m_enabled == false) {
        Mix_HaltMusic();
        Mix_CloseAudio();
    } else {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            LOG_ERROR << "Failed to initialize audio device: " << Mix_GetError() << std::endl;
        }
    }
}

void Audio::play_sound_track(SoundTrack sound_track)
{
    if (!m_enabled) {
        return;
    }

    if (m_sound_tracks[sound_track] == nullptr) {
        switch (sound_track) {
        case DARK_BLUE:
            m_sound_tracks[sound_track] = Mix_LoadMUS(RESOLVE_RESOURCE("audio/music/dark-blue.ogg"));
            break;
        }

        if (m_sound_tracks[sound_track] == nullptr) {
            LOG_ERROR << "Failed to load soundtrack:" << sound_track << std::endl;
            return;
        }
    }

    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(m_sound_tracks[sound_track], -1);
    } else {
        if (Mix_PausedMusic() == 1) {
            Mix_ResumeMusic();
        } else {
            Mix_PauseMusic();
        }
    }
}

Audio::~Audio()
{
    LOG_DEBUG << "Destroying audio" << std::endl;

    for (auto i = 0; i < SOUND_COUNT; ++i) {
        if (m_sounds[i] != nullptr) {
            Mix_FreeChunk(m_sounds[i]);
        }
    }

    for (auto i = 0; i < SOUND_TRACK_COUNT; ++i) {
        if (m_sound_tracks[i] != nullptr) {
            Mix_FreeMusic(m_sound_tracks[i]);
        }
    }
}
