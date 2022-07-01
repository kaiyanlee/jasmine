// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <memory>
#include <stack>
#include <string>

#include "core/timer.h"
#include "graphics/texture.h"

class Game;

// The dialogue controller manages the text displayed at the bottom of the screen. It
// is used for informational messages as well as cutscenes.
class Dialogue
{
public:
    // A notice is a message the player can walk away from. Examples include item
    // descriptions or NPC greetings.
    enum Notice
    {
        NO_NOTICE,
        NOT_ENOUGH_MANA,
        PICK_UP_GOLD,
        NO_ENEMIES_NEARBY,
        ENTER_LEVEL_0,
        ENTER_LEVEL_1,
        ENTER_LEVEL_2,
        ENTER_LEVEL_3,
        ENTER_LEVEL_4,
        ENTER_LEVEL_5,
        FIRE_BALL_ATTACK,
    };

    // An exchange is an uninterruptible sequence of messages. The player will not
    // be able to move away until the exchange is complete.
    enum Exchange
    {
        NO_EXCHANGE,
        TUTORIAL_0
    };

    // Narrator for this message.
    enum Narrator
    {
        TAINA     = 0,
        ELIZABETH = 1,
        RODERICK  = 2,
        RONIN     = 3,
        MARCOS    = 4,
        ANDERS    = 5,
        JASMINE   = 6,
        PHILIP    = 7,
        TAN       = 8,
        AURORA    = 9,
        ALIA      = 10,
        JOSEPH    = 11,
        ANNA      = 12,
        TANYA     = 13,
        KORA      = 14,
        MISTY     = 15,
        SIA       = 16,
        BELLA     = 17,
        MALCOM    = 18,
        TIFFANY   = 19,
        ATHENA    = 20,
        ALAN      = 21,
        HENRY     = 22,
        GEORGE    = 23,
        NONE      = 24,
    };

    // Narrator expression.
    enum Expression
    {
        SAD    = 0,
        NORMAL = 1,
        HAPPY  = 2,
        ANGRY  = 3,
    };

public:
    // Create a new dialogue controller.
    explicit Dialogue(Game& game);

public:
    // Initialize the dialogue controller.
    bool initialize();

    // Play an exchange between characters.
    void play_exchange(Exchange exchange);

    // Return true if an exchange is currently playing.
    bool is_exchange_playing() const;

    // Play a notice.
    void play_notice(Notice notice);

    // Return true if a notice is currently playing.
    bool is_notice_playing() const;

    // Render the text at the bottom of the screen if a notice
    // or exchange is playing.
    void render();

    void pause();
    void resume();
    void next();

private:
    static const auto SPRITESHEET_ROWS    = 24;
    static const auto SPRITESHEET_COLUMNS = 4;
    static const auto SPRITE_WIDTH        = 224;
    static const auto SPRITE_HEIGHT       = 285;

    // Set the text at the bottom of the screen.
    void add_text(Narrator narrator, const std::string& text);
    void add_text(Narrator narrator, const std::initializer_list<std::string>& list);
    void set_text(const std::string& text);
    void set_narrator_name(Narrator narrator);

private:
    Timer m_timer;
    Game& m_game;

    bool m_exchange_playing;
    bool m_notice_playing;

    Texture m_text_texture;
    Texture m_narrator_texture;
    Texture m_portrait_texture;

    std::stack<std::pair<Narrator, std::string>> m_stack;

    int m_text_x;
    int m_text_y;
    int m_narrator_x;
    int m_narrator_y;

    Narrator   m_current_narrator;
    Expression m_current_expression;

    Exchange m_last_exchange;
    Notice   m_last_notice;

    SDL_Rect m_clips[SPRITESHEET_ROWS * SPRITESHEET_COLUMNS];
};
