// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "core/dialogue.h"

#include "core/game.h"

Dialogue::Dialogue(Game& game)
  : m_game(game)
  , m_exchange_playing(false)
  , m_notice_playing(false)
  , m_current_narrator(Narrator::JASMINE)
  , m_current_expression(Expression::SAD)
  , m_last_exchange(Exchange::NO_EXCHANGE)
  , m_last_notice(Notice::NO_NOTICE)
{
    for (int i = 0; i < SPRITESHEET_ROWS; ++i) {
        for (int j = 0, y = i * SPRITE_HEIGHT; j < SPRITESHEET_COLUMNS; ++j) {
            m_clips[j + (i * SPRITESHEET_COLUMNS)] = { j * SPRITE_WIDTH, y, SPRITE_WIDTH, SPRITE_HEIGHT };
        }
    }
}

bool Dialogue::initialize()
{
    if (!m_portrait_texture.load(m_game.window.renderer(),
                                 RESOLVE_RESOURCE("images/characters/portraits/sprites.png"))) {
        LOG_ERROR << "Failed to load profile texture" << std::endl;
        return false;
    }

    return true;
}

void Dialogue::set_text(const std::string& text)
{
    // Load text...
    if (!m_text_texture.load_from_text(m_game.window.renderer(), text.data(), m_game.font(), { 255, 255, 255, 255 })) {
        LOG_ERROR << "Failed to load text texture: " << text.length() << " (text size)" << std::endl;
        return;
    }

    m_text_x = (m_game.window.width() / 2) - m_text_texture.width() / 2;

    // Place text above slots...
    m_text_y = (m_game.window.height() - 74) - m_text_texture.height() / 2;
}

void Dialogue::set_narrator_name(Narrator narrator)
{
    m_current_narrator = narrator;

    std::string name;

    switch (narrator) {
    case TAINA:
        name = "Taina";
        break;
    case ELIZABETH:
        name = "Elizabeth";
        break;
    case RODERICK:
        name = "Roderick";
        break;
    case RONIN:
        name = "Ronin";
        break;
    case MARCOS:
        name = "Marcos";
        break;
    case ANDERS:
        name = "Anders";
        break;
    case JASMINE:
        name = "Jasmine";
        break;
    case PHILIP:
        name = "Philip";
        break;
    case TAN:
        name = "Tan";
        break;
    case AURORA:
        name = "Aurora";
        break;
    case ALIA:
        name = "Alia";
        break;
    case JOSEPH:
        name = "Joseph";
        break;
    case ANNA:
        name = "Anna";
        break;
    case TANYA:
        name = "Tanya";
        break;
    case KORA:
        name = "Kora";
        break;
    case MISTY:
        name = "Misty";
        break;
    case SIA:
        name = "Sia";
        break;
    case BELLA:
        name = "Bella";
        break;
    case MALCOM:
        name = "Malcom";
        break;
    case TIFFANY:
        name = "Tiffany";
        break;
    case ATHENA:
        name = "Athena";
        break;
    case ALAN:
        name = "Alan";
        break;
    case HENRY:
        name = "Henry";
        break;
    case GEORGE:
        name = "George";
        break;
    default:
        break;
    }

    if (name.empty()) {
        return;
    }

    // Load text...
    if (!m_narrator_texture.load_from_text(m_game.window.renderer(), name.data(), m_game.font(),
                                           { 255, 255, 255, 255 })) {
        LOG_ERROR << "Failed to load text texture: " << name.length() << " (text size)" << std::endl;
        return;
    }

    m_narrator_x = (m_game.window.width() / 2) - m_narrator_texture.width() / 2;

    // Place text above slots...
    m_narrator_y = (m_game.window.height() - (74 + 24)) - m_narrator_texture.height() / 2;
}

void Dialogue::add_text(Narrator narrator, const std::string& text)
{
    if (!m_timer.started()) {
        m_stack.push({ narrator, text });
    } else {
        set_text(text);
        set_narrator_name(narrator);
    }

    m_timer.start();
}

void Dialogue::add_text(Narrator narrator, const std::initializer_list<std::string>& list)
{
    for (const auto& a : list) {
        m_stack.push({ narrator, a });
    }

    if (!m_timer.started()) {
        const auto& top = m_stack.top();

        set_text(top.second);
        set_narrator_name(top.first);

        m_stack.pop();
    }

    m_timer.start();
}

void Dialogue::play_exchange(Exchange exchange)
{
    if (m_notice_playing) {

        while (!m_stack.empty()) {
            m_stack.pop();
        }

        m_timer.stop();
    }

    if (m_exchange_playing && m_last_exchange == exchange) {
        return;
    }

    m_exchange_playing = true;
    m_last_exchange    = exchange;

    switch (exchange) {
    case TUTORIAL_0:
        add_text(Narrator::MALCOM, { "This is a gold bar! This is also an example of a dialogue!" });
        add_text(Narrator::JASMINE, { "Understood!" });
        break;
    default:
        break;
    }
}

bool Dialogue::is_exchange_playing() const
{
    return m_exchange_playing;
}

void Dialogue::play_notice(Notice notice)
{
    if (m_notice_playing && m_last_notice == notice) {
        m_timer.start();
        return;
    }

    m_notice_playing = true;
    m_last_notice    = notice;

    m_timer.stop();

    while (!m_stack.empty()) {
        m_stack.pop();
    }

    switch (notice) {
    case NOT_ENOUGH_MANA:
        add_text(Narrator::NONE, { "You lack enough mana to use this skill!" });
        break;
    case FIRE_BALL_ATTACK:
        add_text(Narrator::NONE, { "Fire Ball! Burn thy enemies to dust!" });
        break;
    case PICK_UP_GOLD:
        add_text(Narrator::NONE, { "Try to do your best today, okay?" });
        break;
    case NO_ENEMIES_NEARBY:
        add_text(Narrator::NONE, { "There are no enemies close enough to attack!" });
        break;
    case ENTER_LEVEL_0:
        add_text(Narrator::NONE, { "You have entered level 0: Tutorial" });
        break;
    case ENTER_LEVEL_1:
        add_text(Narrator::NONE, { "You have entered level 1: Town" });
        break;
    case ENTER_LEVEL_2:
        add_text(Narrator::NONE, { "You have entered level 2: Grasslands" });
        break;
    case ENTER_LEVEL_3:
        add_text(Narrator::NONE, { "You have entered level 3: Future" });
        break;
    case ENTER_LEVEL_4:
        add_text(Narrator::NONE, { "You have entered level 4: Dungeons" });
        break;
    case ENTER_LEVEL_5:
        add_text(Narrator::NONE, { "You have entered level 5: << Final Battle >>" });
        break;
    default:
        break;
    }
}

bool Dialogue::is_notice_playing() const
{
    return m_notice_playing;
}

void Dialogue::pause()
{
    m_timer.pause();
}

void Dialogue::resume()
{
    m_timer.resume();
}

void Dialogue::next()
{
    // goto next...
}

void Dialogue::render()
{
    if (!m_timer.started()) {
        return;
    }

    if (m_exchange_playing) {

        const SDL_Rect rect = { 0, m_game.window.height() - SPRITE_HEIGHT / 2, m_game.window.width(),
                                SPRITE_HEIGHT / 2 };

        SDL_SetRenderDrawColor(m_game.window.renderer(), 0, 0, 0, 225);
        SDL_SetRenderDrawBlendMode(m_game.window.renderer(), SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(m_game.window.renderer(), &rect);

        int x;

        switch (m_current_narrator) {
        case TAINA:
        case RONIN:
        case ELIZABETH:
        case ANDERS:
        case PHILIP:
        case TAN:
        case AURORA:
        case ANNA:
        case KORA:
        case MISTY:
        case SIA:
        case BELLA:
            x = m_game.window.width() - SPRITE_WIDTH;
            break;
        case RODERICK:
        case MARCOS:
        case JASMINE:
        case ALIA:
        case JOSEPH:
        case TANYA:
        case MALCOM:
        case TIFFANY:
        case ATHENA:
        case ALAN:
        case HENRY:
        case GEORGE:
        case NONE:
            x = 0;
            break;
        }

        m_portrait_texture.render(x, m_game.window.height() - SPRITE_HEIGHT,
                                  &m_clips[m_current_expression + (m_current_narrator * SPRITESHEET_COLUMNS)]);

        m_narrator_texture.render(m_narrator_x, m_narrator_y);
    }

    m_text_texture.render(m_text_x, m_text_y);

    if (m_timer.ticks() > 5000) {
        if (m_stack.empty()) {
            m_timer.stop();
            m_exchange_playing = false;
            m_notice_playing   = false;
        } else {
            const auto& top = m_stack.top();

            set_narrator_name(top.first);
            set_text(top.second);

            m_stack.pop();
            m_timer.start();
        }
    }
}
