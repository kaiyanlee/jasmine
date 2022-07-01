// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL_mixer.h>

#include "core/common.h"

// The Audio class manages the background music and effect sounds.
class Audio
{
public:
    // Sound effect.
    enum Sound : uint8_t
    {
        BELT_HANDLE1          = 0,
        BELT_HANDLE2          = 1,
        BOOK_CLOSE            = 2,
        BOOK_FLIP1            = 3,
        BOOK_FLIP2            = 4,
        BOOK_FLIP3            = 5,
        BOOK_OPEN             = 6,
        BOOK_PLACE1           = 7,
        BOOK_PLACE2           = 8,
        BOOK_PLACE3           = 9,
        CHOP                  = 10,
        CLOTH1                = 11,
        CLOTH2                = 12,
        CLOTH3                = 13,
        CLOTH4                = 14,
        CLOTH_BELT2           = 15,
        CLOTH_BELT            = 16,
        CREAK1                = 17,
        CREAK2                = 18,
        CREAK3                = 19,
        DOOR_CLOSE_1          = 20,
        DOOR_CLOSE_2          = 21,
        DOOR_CLOSE_3          = 22,
        DOOR_CLOSE_4          = 23,
        DOOR_OPEN_1           = 24,
        DOOR_OPEN_2           = 25,
        DRAW_KNIFE1           = 26,
        DRAW_KNIFE2           = 27,
        DRAW_KNIFE3           = 28,
        DROP_LEATHER          = 29,
        FOOTSTEP00            = 30,
        FOOTSTEP01            = 31,
        FOOTSTEP02            = 32,
        FOOTSTEP03            = 33,
        FOOTSTEP04            = 34,
        FOOTSTEP05            = 35,
        FOOTSTEP06            = 36,
        FOOTSTEP07            = 37,
        FOOTSTEP08            = 38,
        FOOTSTEP09            = 39,
        HANDLE_COINS2         = 40,
        HANDLE_COINS          = 41,
        HANDLE_SMALL_LEATHER2 = 42,
        HANDLE_SMALL_LEATHER  = 43,
        KNIFE_SLICE2          = 44,
        KNIFE_SLICE           = 45,
        METAL_CLICK           = 46,
        METAL_LATCH           = 47,
        METAL_POT1            = 48,
        METAL_POT2            = 49,
        METAL_POT3            = 50,
    };

    // Background music.
    enum SoundTrack : uint8_t
    {
        DARK_BLUE = 0,
    };

public:
    // Create a new Audio class.
    explicit Audio();

    // Destructor.
    ~Audio();

public:
    // Play the following track (looped).
    void play_sound_track(SoundTrack sound_track);

    // Play the following sound effect with its number of loops.
    void play_sound(Sound sound, int loops);

    // Return true if music and sound is enabled.
    bool enabled() const;

    // Enable music and sound.
    void set_enabled(bool enabled);

    // Mute all audio output.
    void toggle_mute();

    // Return true if audio is muted.
    bool muted() const;

private:
    bool       m_enabled;
    Mix_Music* m_sound_tracks[SOUND_TRACK_COUNT];
    Mix_Chunk* m_sounds[SOUND_COUNT];
};
