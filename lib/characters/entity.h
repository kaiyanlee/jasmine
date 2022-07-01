// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <SDL2/SDL.h>

#include <memory>
#include <vector>

#include "characters/inventory.h"
#include "characters/skill.h"
#include "core/common.h"
#include "core/logger.h"
#include "core/math.h"
#include "core/timer.h"
#include "graphics/texture.h"

class Game;

// The entity class represents a character. This includes the player, NPCs, and all enemies. For convenience,
// we have chosen to keep everything together instead of using inheritance for individual abilities.
class Entity
{
public:
    // The type of entity.
    enum Type
    {
        PLAYER_TYPE = 0,
        NPC_TYPE    = 1,
        ENEMY_TYPE  = 2,
    };

    // The direction of the entity.
    enum Direction
    {
        LEFT  = 1 << 0,
        RIGHT = 1 << 1,
        UP    = 1 << 2,
        DOWN  = 1 << 3,
    };

    // The sprite used by the entity.
    enum Sprite
    {
        SPRITE_PLAYER            = 0,
        SPRITE_BOAR_MAN          = 1,
        SPRITE_BOAR_MAN_GUARDIAN = 2,
        SPRITE_BOAR_MAN_BOSS     = 3,
    };

public:
    // Create a new entity of the given type for a game.
    explicit Entity(Type type, Game& game);

    // Entity destructor.
    ~Entity();

public:
    // Return the id of the entity.
    int id() const;

    // Set the entity id.
    void set_id(int id);

    // Set the current sprite for the entity.
    bool set_sprite(Sprite sprite);

    // Return the sprite of the entity.
    Sprite sprite() const;

    // Return the inventory of the entity.
    const Inventory& inventory() const;

    // Set the inventory for the entity.
    void set_inventory(const Inventory& inventory);

    // Return the x and y position
    float pos_y() const;
    float pos_x() const;

    int health() const;
    int health_bars() const;
    int max_health() const;

    int mana() const;
    int mana_bars() const;
    int max_mana() const;

    int stamina() const;
    int stamina_bars() const;
    int max_stamina() const;

    void set_health(int health);
    void set_mana(int mana);
    void set_stamina(int stamina);

    bool is_near_entity(Entity* entity);

    // Update the position.
    void update(uint32_t ticks);

    // Damage the entity.
    void damage(int amount);

    // Render with camera.
    void render(const SDL_Rect& camera);

    // Check for collisions.
    void check_collision();

    /// Return true if entity is dead.
    bool dead() const;

    // Walk to position.
    void walk_to_position(int x, int y);

    // Set the position.
    void set_position(int x, int y);

    // Return true if currently attacking a target.
    bool attacking() const;

    // Return the skills.
    const std::vector<Skill>& skills() const;

    int  column() const;
    int  row() const;
    bool is_player() const;

    void attack(Entity* entity);

    // Enable auto-attack.
    void auto_attack();

    void stop_attacking();

    // Walk in direction.
    void walk_in_direction(Direction direction);

    // Stop walking in direction.
    void stop_walking_in_direction(Direction direction);

    // Stop walking in any direction.
    void stop_walking();

    void fall();
    void jump();

    bool is_moving(Direction direction) const;

    void use_skill(int skill_number);

private:
    enum class Action
    {
        OPEN_ARMS,
        SPEAR_ATTACK,
        WALK,
        SLASH,
        FALL,
        WIDE_SLASH,
    };

    enum State : uint8_t
    {
        OPEN_ARMS_UP       = 0,
        OPEN_ARMS_LEFT     = 1,
        OPEN_ARMS_DOWN     = 2,
        OPEN_ARMS_RIGHT    = 3,
        SPEAR_ATTACK_UP    = 4,
        SPEAR_ATTACK_LEFT  = 5,
        SPEAR_ATTACK_DOWN  = 6,
        SPEAR_ATTACK_RIGHT = 7,
        WALK_UP            = 8,
        WALK_LEFT          = 9,
        WALK_DOWN          = 10,
        WALK_RIGHT         = 11,
        SLASH_ATTACK_UP    = 12,
        SLASH_ATTACK_LEFT  = 13,
        SLASH_ATTACK_DOWN  = 14,
        SLASH_ATTACK_RIGHT = 15,
        ARROW_ATTACK_UP    = 16,
        ARROW_ATTACK_LEFT  = 17,
        ARROW_ATTACK_DOWN  = 18,
        ARROW_ATTACK_RIGHT = 19,
        FALL               = 20,
        WIDE_SLASH_UP      = 21,
        WIDE_SLASH_DOWN    = 22,
        WIDE_SLASH_LEFT    = 23,
        WIDE_SLASH_RIGHT   = 24,
    };

    static const int CHARACTER_WIDE_WIDTH        = CHARACTER_WIDTH * 3;
    static const int CHARACTER_WIDE_HEIGHT       = CHARACTER_HEIGHT * 3;
    static const int CHARACTER_HORIZONTAL_CENTER = CHARACTER_WIDTH / 2;
    static const int CHARACTER_VERTICAL_CENTER   = CHARACTER_HEIGHT / 2;

    static const int CHARACTER_BOUNDING_BOX_LEFT   = 16;
    static const int CHARACTER_BOUNDING_BOX_RIGHT  = CHARACTER_WIDTH - 16;
    static const int CHARACTER_BOUNDING_BOX_TOP    = CHARACTER_HEIGHT / 2;
    static const int CHARACTER_BOUNDING_BOX_BOTTOM = CHARACTER_HEIGHT;

    static const int JUMP_HEIGHT = CHARACTER_HEIGHT * 10;

    // Split the sprites into individual rectangles for clipping.
    static const std::vector<SDL_Rect> split(uint8_t index, uint8_t frames)
    {
        std::vector<SDL_Rect> vec(frames);

        // Each sprite is 64x64 so we have to multiply to get the y.
        int y = CHARACTER_HEIGHT * index;

        for (int i = 0; i < frames; ++i) {
            vec[i] = { i * CHARACTER_WIDTH, y, CHARACTER_WIDTH, CHARACTER_HEIGHT };
        }

        return vec;
    }

    static const std::vector<SDL_Rect> split_wide(uint8_t index, uint8_t frames)
    {
        std::vector<SDL_Rect> vec(frames);

        int y = (CHARACTER_HEIGHT * WIDE_SLASH_UP) + ((index - WIDE_SLASH_UP) * CHARACTER_WIDE_HEIGHT);

        for (int i = 0; i < frames; ++i) {
            vec[i] = { i * CHARACTER_WIDE_WIDTH, y, CHARACTER_WIDE_WIDTH, CHARACTER_WIDE_HEIGHT };
        }

        return vec;
    }

    // Return the sprite rectangles for each state.
    static const std::array<std::vector<SDL_Rect>, 25>& states()
    {
        static const std::array<std::vector<SDL_Rect>, 25> clips = {
            split(OPEN_ARMS_UP, 7),
            split(OPEN_ARMS_LEFT, 7),
            split(OPEN_ARMS_DOWN, 7),
            split(OPEN_ARMS_RIGHT, 7),
            split(SPEAR_ATTACK_UP, 8),
            split(SPEAR_ATTACK_LEFT, 8),
            split(SPEAR_ATTACK_DOWN, 8),
            split(SPEAR_ATTACK_RIGHT, 8),
            split(WALK_UP, 9),
            split(WALK_LEFT, 9),
            split(WALK_DOWN, 9),
            split(WALK_RIGHT, 9),
            split(SLASH_ATTACK_UP, 6),
            split(SLASH_ATTACK_LEFT, 6),
            split(SLASH_ATTACK_DOWN, 6),
            split(SLASH_ATTACK_RIGHT, 6),
            split(ARROW_ATTACK_UP, 13),
            split(ARROW_ATTACK_LEFT, 13),
            split(ARROW_ATTACK_DOWN, 13),
            split(ARROW_ATTACK_RIGHT, 13),
            split(FALL, 6),
            split_wide(WIDE_SLASH_UP, 6),
            split_wide(WIDE_SLASH_LEFT, 6),
            split_wide(WIDE_SLASH_DOWN, 6),
            split_wide(WIDE_SLASH_RIGHT, 6),
        };

        return clips;
    };

private:
    void stop_moving_horizontally();
    void stop_moving_vertically();
    void start_animation();
    void stop_animation();
    void face_towards_entity(Entity* entity);
    void finish_jump();

    void set_action(Action action);

private:
    Vector3D<float> m_position;
    Vector3D<float> m_velocity;
    Vector3D<float> m_acceleration;
    Vector2D<int>   m_virtual;

    int m_health;
    int m_health_bars;
    int m_max_health;

    int m_mana;
    int m_mana_bars;
    int m_max_mana;

    int m_stamina;
    int m_stamina_bars;
    int m_max_stamina;

    float m_speed;
    int   m_max_speed;

    Texture* m_texture;

    const std::array<std::vector<SDL_Rect>, 25>& m_states;
    int                                          m_current_state;

    size_t m_current_frame;

    Timer m_timer;

    int       m_movement_direction;
    Direction m_sprite_direction;

    Game& m_game;

    bool m_attacking;

    int m_col;
    int m_row;

    Type m_type;

    int m_attack_long_range;
    int m_attack_short_range;

    int m_attack_power;

    Entity* m_target;

    Vector2D<int> m_destination;
    bool          m_walking_to_destination;

    bool m_jumping;

    Inventory m_inventory;

    int m_animation_rate;

    std::vector<Skill> m_skills;

    std::vector<std::unique_ptr<Texture>> m_damage_stack;

    Sprite m_sprite;
};
