// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "characters/entity.h"

#include <SDL_render.h>

#include "core/game.h"
#include "core/logger.h"

Entity::Entity(Type type, Game& game)
  : m_position({ 0.0, 0.0, 0.0 })
  , m_velocity({ 0.0, 0.0, 0.0 })
  , m_acceleration({ 0.0, 0.0, 0.0 })
  , m_health(1000)
  , m_health_bars(BAR_COUNT)
  , m_max_health(1000)
  , m_mana(1000)
  , m_mana_bars(BAR_COUNT)
  , m_max_mana(1000)
  , m_stamina(1000)
  , m_stamina_bars(BAR_COUNT)
  , m_max_stamina(1000)
  , m_speed(12.0)
  , m_max_speed(244)
  , m_texture(nullptr)
  , m_states(states())
  , m_current_state(State::WALK_DOWN)
  , m_current_frame(0)
  , m_movement_direction(0)
  , m_sprite_direction(Direction::DOWN)
  , m_game(game)
  , m_attacking(false)
  , m_col(0)
  , m_row(0)
  , m_type(type)
  , m_attack_long_range(10)
  , m_attack_short_range(3)
  , m_attack_power(100)
  , m_target(nullptr)
  , m_destination({ 0, 0 })
  , m_walking_to_destination(false)
  , m_jumping(false)
  , m_animation_rate(16)
  , m_sprite(SPRITE_PLAYER)
{
    m_skills.push_back(Skill(Skill::FIRE_MAGIC_251, 1));
    m_skills.push_back(Skill(Skill::FIRE_MAGIC_252, 1));
    m_skills.push_back(Skill(Skill::FIRE_MAGIC_253, 1));
    m_skills.push_back(Skill(Skill::FIRE_MAGIC_254, 1));
    m_skills.push_back(Skill(Skill::FIRE_MAGIC_255, 1));
    m_skills.push_back(Skill(Skill::FIRE_MAGIC_256, 1));
    m_skills.push_back(Skill(Skill::FIRE_MAGIC_257, 1));

    m_skills.push_back(Skill(Skill::POTION_147, 1));
    m_skills.push_back(Skill(Skill::POTION_150, 1));
    m_skills.push_back(Skill(Skill::POTION_158, 1));
}

bool Entity::set_sprite(Sprite sprite)
{
    if (m_sprite == sprite && m_texture != nullptr) {
        return true;
    }

    m_texture = m_game.get_entity_texture(sprite);

    if (m_texture == nullptr) {
        return false;
    }

    m_sprite = sprite;

    return true;
}

Entity::Sprite Entity::sprite() const
{
    return m_sprite;
}

void Entity::update(uint32_t ticks)
{
    if (m_timer.started()) {
        const auto ticks = m_timer.ticks();

        // Dead now...
        if (m_current_state == State::FALL && m_current_frame == 5) {
            return stop_animation();
        }

        // Update animation frame.
        m_current_frame = (ticks * m_animation_rate) / 1000 % m_states[m_current_state].size();

        // Continue walking left/right.
        if (m_velocity.x > 0.0) {
            m_velocity.x += m_speed * (ticks / 1000.0f);
        } else if (m_velocity.x < 0.0) {
            m_velocity.x -= m_speed * (ticks / 1000.0f);
        }

        // Continue walking up/down.
        if (m_velocity.y > 0.0) {
            m_velocity.y += m_speed * (ticks / 1000.0f);
        } else if (m_velocity.y < 0.0) {
            m_velocity.y -= m_speed * (ticks / 1000.0f);
        }

        if (ticks >= 1000) {
            if (m_attacking) {
                if (m_target->dead()) {
                    stop_attacking();
                } else {
                    // Face towards target.
                    face_towards_entity(m_target);

                    // Damage the target.
                    m_target->damage(rand() % m_attack_power);
                }
            }

            // Restart the timer.
            m_timer.start();
        }
    }

    if (m_walking_to_destination) {
        int px = m_position.x;
        int py = m_position.y;
        int dx = m_destination.x;
        int dy = m_destination.y;

        // LOG_DEBUG << "Destination: "
        //           << " " << dx << " " << dy << " " << px << " " << py << "\n";

        if (dx < px) {
            walk_in_direction(Direction::LEFT);
        } else if (dx > px) {
            walk_in_direction(Direction::RIGHT);
        }

        if (dy < py) {
            walk_in_direction(Direction::UP);
        } else if (dy > py) {
            walk_in_direction(Direction::DOWN);
        }

        m_velocity.x = (dx - px) * m_speed;
        m_velocity.y = (dy - py) * m_speed;

        if (px == dx && py == dy) {
            // We have reached our destination now.
            m_walking_to_destination = false;

            // Stop walking animation.
            stop_walking();

            m_sprite_direction = Direction::UP;

            m_velocity.x = 0;
            m_velocity.y = 0;

            return;
        }
    }

    // Entity has stopped moving.
    if (m_velocity.x == 0.0 && m_velocity.y == 0.0 && m_velocity.z == 0.0) {
        return;
    }

    const auto dt = (ticks / 1000.0f);

    if (m_velocity.x > m_max_speed) {
        m_velocity.x = m_max_speed;
    } else if (m_velocity.x < -m_max_speed) {
        m_velocity.x = -m_max_speed;
    }

    // Do not exceed maximum Y speed.
    if (m_velocity.y > m_max_speed) {
        m_velocity.y = m_max_speed;
    } else if (m_velocity.y < -m_max_speed) {
        m_velocity.y = -m_max_speed;
    }

    // LOG_DEBUG << dt << "\n";

    LOG_DEBUG << "Velocity Y: " << m_velocity.y << " Velocity X: " << m_velocity.x << " Velocity Z: " << m_velocity.z
              << " DT: " << dt << "\n";

    m_position.x += m_velocity.x * dt;

    if (m_position.x < 0) {
        m_position.x = 0;
    } else if (m_position.x + CHARACTER_WIDTH > MAP_WIDTH) {
        m_position.x = MAP_WIDTH - CHARACTER_WIDTH;
    }

    m_position.y += m_velocity.y * dt;

    if (m_position.y < 0) {
        m_position.y = 0;
    } else if (m_position.y + CHARACTER_HEIGHT > MAP_HEIGHT) {
        m_position.y = MAP_HEIGHT - CHARACTER_HEIGHT;
    }

    if (m_jumping) {
        // LOG_DEBUG << "JUMP: " << m_position.z << "\n";

        m_position.z += (m_velocity.z * dt);

        if (m_position.z < 0) {
            m_position.z = 0;
        } else if (m_position.z + CHARACTER_HEIGHT > MAP_HEIGHT) {
            m_position.z = MAP_HEIGHT - CHARACTER_HEIGHT;
        }

        // On the ground again.
        if (m_position.z >= m_position.y) {
            finish_jump();
        } else {
            m_velocity.z += m_acceleration.z * dt;
        }
    } else {
        m_position.z = m_position.y;
    }
}

int Entity::row() const
{
    return m_row;
}

int Entity::column() const
{
    return m_col;
}

bool Entity::attacking() const
{
    return m_attacking;
}

void Entity::start_animation()
{
    if (!m_timer.started()) {
        m_timer.start();
    }
}

void Entity::stop_animation()
{
    m_timer.stop();
}

bool Entity::is_near_entity(Entity* entity)
{
    return entity->column() > m_col - m_attack_short_range && entity->column() < m_col + m_attack_short_range
           && entity->row() > m_row - m_attack_short_range && entity->row() < m_row + m_attack_short_range;
}

void Entity::damage(int amount)
{
    if (m_health <= 0) {
        return;
    }

    LOG_DEBUG << "Entity received damaged:  " << amount << "\n";

    set_health(m_health - amount);

    if (m_health <= 0) {
        fall();
    }

    auto texture = m_damage_stack.insert(m_damage_stack.end(), std::make_unique<Texture>())->get();

    if (!texture->load_from_text(m_game.window.renderer(), std::to_string(amount).c_str(), m_game.font(),
                                 { 255, 255, 255, 255 })) {
        LOG_ERROR << "Failed to load damage texture text" << std::endl;
        return;
    }

    texture->set_scale(std::max(1.0, std::min(2.0, (static_cast<float>(amount) / static_cast<float>(100)) * 2.0)));
    texture->set_rotation(rand() % 20);
}

void Entity::face_towards_entity(Entity* entity)
{
    LOG_DEBUG << entity->pos_y() << " " << m_position.y << "\n";

    if (entity->pos_y() > m_position.y) {
        m_sprite_direction = Direction::DOWN;
    } else if (entity->pos_y() < m_position.y) {
        m_sprite_direction = Direction::UP;
    } else if (entity->pos_x() > m_position.x) {
        m_sprite_direction = Direction::RIGHT;
    } else if (entity->pos_x() < m_position.x) {
        m_sprite_direction = Direction::LEFT;
    }
}

bool Entity::dead() const
{
    return m_health <= 0;
}

void Entity::auto_attack()
{
    if (m_attacking) {
        return;
    }

    SDL_Rect range;
    range.x = (m_col - m_attack_long_range) * MAP_TILE_SIZE;
    range.y = (m_row - m_attack_long_range) * MAP_TILE_SIZE;
    range.w = (m_col + m_attack_long_range) * MAP_TILE_SIZE;
    range.h = (m_row + m_attack_long_range) * MAP_TILE_SIZE;

    SDL_Rect result;

    auto entities = std::vector<Entity*>(m_game.map.entities());

    // Sort the entities by distance from us.
    std::sort(entities.begin(), entities.end(), [this](Entity* a, Entity* b) {
        return (std::sqrt(std::pow(a->pos_x() - this->pos_x(), 2) + std::pow(a->pos_y() - this->pos_y(), 2)))
               < (std::sqrt(std::pow(b->pos_x() - this->pos_x(), 2) + std::pow(b->pos_y() - this->pos_y(), 2)));
    });

    for (auto& entity : entities) {
        if (entity != this && !entity->dead()) {
            SDL_Rect box;

            box.x = entity->pos_x() - CHARACTER_WIDTH;
            box.y = entity->pos_y() - CHARACTER_HEIGHT;
            box.w = CHARACTER_WIDTH;
            box.h = CHARACTER_HEIGHT;

            if (SDL_IntersectRect(&range, &box, &result)) {
                return attack(entity);
            }
        }
    }

    m_game.dialogue.play_notice(Dialogue::Notice::NO_ENEMIES_NEARBY);
}

void Entity::attack(Entity* entity)
{
    if (m_attacking) {
        return;
    }

    if (entity->dead()) {
        return;
    }

    // Update the target.
    m_target = entity;

    // walk_to_position(entity->pos_x() + CHARACTER_HORIZONTAL_CENTER, entity->pos_y());

    m_game.audio.play_sound(Audio::Sound::KNIFE_SLICE2, 0);
    m_game.audio.play_sound(Audio::Sound::KNIFE_SLICE2, -1);

    m_attacking = true;

    m_velocity.x = 0;
    m_velocity.y = 0;

    // Start attacking animation.
    start_animation();

    set_action(Action::WIDE_SLASH);
}

void Entity::stop_attacking()
{
    m_attacking     = false;
    m_current_frame = 0;
    m_velocity.x    = 0;
    m_velocity.y    = 0;

    // Stop attacking animation.
    stop_animation();
}

void Entity::stop_walking_in_direction(Direction direction)
{
    if (direction == Direction::UP || direction == Direction::DOWN) {
        stop_moving_vertically();
    } else {
        stop_moving_horizontally();
    }
}

void Entity::stop_walking()
{
    stop_moving_horizontally();
    stop_moving_vertically();
}

bool Entity::is_moving(Direction direction) const
{
    return (m_movement_direction & direction) == direction;
}

void Entity::stop_moving_horizontally()
{
    LOG_DEBUG << "Stopped walking horizontally\n";

    if (m_attacking) {
        stop_attacking();
    } else {
        m_velocity.x    = 0;
        m_current_frame = 0;
    }

    if (m_velocity.y == 0.0) {
        stop_animation();
    }

    if (is_moving(Direction::LEFT)) {
        m_movement_direction &= ~Direction::LEFT;
    } else if (is_moving(Direction::RIGHT)) {
        m_movement_direction &= ~Direction::RIGHT;
    }

    if (is_moving(Direction::UP)) {
        m_current_state    = State::WALK_UP;
        m_sprite_direction = Direction::UP;
    } else if (is_moving(Direction::DOWN)) {
        m_current_state    = State::WALK_DOWN;
        m_sprite_direction = Direction::DOWN;
    }
}

void Entity::stop_moving_vertically()
{
    LOG_DEBUG << "Stopped walking vertically\n";

    if (m_attacking) {
        stop_attacking();
    } else {
        m_velocity.z    = 0;
        m_velocity.y    = 0;
        m_current_frame = 0;
    }

    if (m_velocity.x == 0.0) {
        stop_animation();
    }

    if (is_moving(Direction::UP)) {
        m_movement_direction &= ~Direction::UP;
    } else if (is_moving(Direction::DOWN)) {
        m_movement_direction &= ~Direction::DOWN;
    }

    if (is_moving(Direction::RIGHT)) {
        m_current_state    = State::WALK_RIGHT;
        m_sprite_direction = Direction::RIGHT;
    } else if (is_moving(Direction::LEFT)) {
        m_current_state    = State::WALK_LEFT;
        m_sprite_direction = Direction::LEFT;
    }
}

void Entity::walk_in_direction(Direction direction)
{
    switch (direction) {
    case LEFT:
        if (is_moving(Direction::RIGHT)) {
            stop_moving_horizontally();
        }

        if (m_current_state != State::WALK_LEFT) {
            m_current_state    = State::WALK_LEFT;
            m_sprite_direction = Direction::LEFT;
        }

        if (is_moving(Direction::LEFT)) {
            return;
        }

        LOG_DEBUG << "Started walking left\n";

        m_movement_direction |= Direction::LEFT;
        m_velocity.x = -m_speed;

        break;
    case RIGHT:
        if (is_moving(Direction::LEFT)) {
            stop_moving_horizontally();
        }

        if (m_current_state != State::WALK_RIGHT) {
            m_current_state    = State::WALK_RIGHT;
            m_sprite_direction = Direction::RIGHT;
        }

        if (is_moving(Direction::RIGHT)) {
            return;
        }

        LOG_DEBUG << "Started walking right\n";

        m_movement_direction |= Direction::RIGHT;
        m_velocity.x = m_speed;

        break;
    case UP:
        if (m_current_state != State::WALK_UP && !is_moving(Direction::LEFT) && !is_moving(Direction::RIGHT)) {
            m_current_state    = State::WALK_UP;
            m_sprite_direction = Direction::UP;
        }

        if (is_moving(Direction::UP)) {
            return;
        }

        LOG_DEBUG << "Started walking up\n";

        m_velocity.y = -m_speed;
        m_movement_direction |= Direction::UP;

        break;
    case DOWN:
        if (m_current_state != State::WALK_DOWN && !is_moving(Direction::LEFT) && !is_moving(Direction::RIGHT)) {
            m_current_state    = State::WALK_DOWN;
            m_sprite_direction = Direction::DOWN;
        }

        if (is_moving(Direction::DOWN)) {
            return;
        }

        LOG_DEBUG << "Started walking down\n";

        m_velocity.y = m_speed;
        m_movement_direction |= Direction::DOWN;

        break;
    }

    // Play footstep sounds.
    m_game.audio.play_sound(Audio::Sound::FOOTSTEP02, 0);

    // Start character walking animation.
    start_animation();
}

void Entity::fall()
{
    if (m_current_state == State::FALL) {
        return;
    }

    m_current_state = State::FALL;

    start_animation();
}

float Entity::pos_x() const
{
    return m_position.x;
}

float Entity::pos_y() const
{
    return m_position.y;
}

bool Entity::is_player() const
{
    return m_type == PLAYER_TYPE;
}

void Entity::set_health(int health)
{
    m_health      = health;
    m_health_bars = (static_cast<float>(m_health) / static_cast<float>(m_max_health)) * BAR_COUNT;
}

void Entity::set_mana(int mana)
{
    m_mana      = mana;
    m_mana_bars = (static_cast<float>(m_mana) / static_cast<float>(m_max_mana)) * BAR_COUNT;
}

void Entity::set_stamina(int stamina)
{
    m_stamina      = stamina;
    m_stamina_bars = (static_cast<float>(m_stamina) / static_cast<float>(m_max_stamina)) * BAR_COUNT;
}

void Entity::set_position(int x, int y)
{
    m_position.x = x - (CHARACTER_WIDTH / 2);
    m_position.y = y - (CHARACTER_HEIGHT / 2);
    m_position.z = m_position.y;
}

void Entity::walk_to_position(int x, int y)
{
    m_destination.x = x - (CHARACTER_WIDTH / 2);
    m_destination.y = y - (CHARACTER_HEIGHT / 2);

    m_walking_to_destination = true;
}

void Entity::jump()
{
    if (m_jumping || m_velocity.y != 0.0) {
        return;
    }

    m_velocity.z -= sqrt(2.0f * GRAVITY * JUMP_HEIGHT);
    m_acceleration.z = -m_velocity.z;

    m_jumping        = true;
    m_animation_rate = 60;

    start_animation();

    if ((m_movement_direction & Direction::RIGHT) == Direction::RIGHT) {
        m_current_state    = State::WALK_RIGHT;
        m_sprite_direction = Direction::RIGHT;
    } else if ((m_movement_direction & Direction::LEFT) == Direction::LEFT) {
        m_current_state    = State::WALK_LEFT;
        m_sprite_direction = Direction::LEFT;
    } else if ((m_movement_direction & Direction::UP) == Direction::UP) {
        m_current_state    = State::WALK_UP;
        m_sprite_direction = Direction::UP;
    } else if ((m_movement_direction & Direction::DOWN) == Direction::DOWN) {
        m_current_state    = State::WALK_DOWN;
        m_sprite_direction = Direction::DOWN;
    }
}

void Entity::finish_jump()
{
    m_jumping = false;

    m_position.z = m_position.y;
    m_velocity.z = 0;

    m_current_frame = 0;

    if (m_velocity.x == 0.0 && m_velocity.y == 0.0) {
        stop_animation();
    }

    m_animation_rate = 16;
}

int Entity::health() const
{
    return m_health;
}

int Entity::health_bars() const
{
    return m_health_bars;
}

int Entity::max_health() const
{
    return m_max_health;
}

int Entity::mana() const
{
    return m_mana;
}

int Entity::mana_bars() const
{
    return m_mana_bars;
}

int Entity::max_mana() const
{
    return m_max_mana;
}

int Entity::stamina() const
{
    return m_stamina;
}

int Entity::stamina_bars() const
{
    return m_stamina_bars;
}

int Entity::max_stamina() const
{
    return m_max_stamina;
}

const std::vector<Skill>& Entity::skills() const
{
    return m_skills;
}

void Entity::check_collision()
{
    switch (m_sprite_direction) {
    case Direction::LEFT:
        m_col = (m_position.x + CHARACTER_BOUNDING_BOX_LEFT) / MAP_TILE_SIZE;
        m_row = (m_position.y + CHARACTER_BOUNDING_BOX_BOTTOM) / MAP_TILE_SIZE;
        break;
    case Direction::RIGHT:
        m_col = (m_position.x + CHARACTER_BOUNDING_BOX_RIGHT) / MAP_TILE_SIZE;
        m_row = (m_position.y + CHARACTER_BOUNDING_BOX_BOTTOM) / MAP_TILE_SIZE;
        break;
    case Direction::UP:
        m_col = (m_position.x + CHARACTER_BOUNDING_BOX_LEFT) / MAP_TILE_SIZE;
        m_row = (m_position.y + CHARACTER_BOUNDING_BOX_BOTTOM) / MAP_TILE_SIZE;
        break;
    case Direction::DOWN:
        m_col = (m_position.x + CHARACTER_BOUNDING_BOX_RIGHT) / MAP_TILE_SIZE;
        m_row = (m_position.y + CHARACTER_BOUNDING_BOX_BOTTOM) / MAP_TILE_SIZE;
        break;
    }

    if (auto& tile = m_game.map.at(m_row, m_col); tile.has_fg()) {
        if (is_player()) {
            switch (tile.fg()) {
            case Tile::SMALL_GOLD_BAR_1:
            case Tile::SMALL_GOLD_BAR_2:
            case Tile::SMALL_GOLD_BAR_3:
            case Tile::SMALL_GOLD_BAR_4:
            case Tile::SMALL_GOLD_BAR_5:
            case Tile::BIG_GOLD_BAR_1:
            case Tile::BIG_GOLD_BAR_2:
            case Tile::BIG_GOLD_BAR_3:
                m_inventory.add_gold(1);
                tile.set_fg(-1);
                m_game.dialogue.play_exchange(Dialogue::TUTORIAL_0);
                return;
            case Tile::DOOR_1:
            case Tile::DOOR_2:
                m_game.map.go_to_next_level();
                break;
            default:
                break;
            }
        }

        LOG_DEBUG << "Hit!\n";

        switch (m_sprite_direction) {
        case Direction::LEFT:
            m_position.x = ((m_col + 1) * MAP_TILE_SIZE) - (CHARACTER_BOUNDING_BOX_LEFT + 1);
            break;
        case Direction::RIGHT:
            m_position.x = (m_col * MAP_TILE_SIZE) - (CHARACTER_BOUNDING_BOX_RIGHT + 1);
            break;
        case Direction::UP:
            m_position.y = ((m_row + 1) * MAP_TILE_SIZE) - (CHARACTER_BOUNDING_BOX_TOP + 1);
            break;
        case Direction::DOWN:
            m_position.y = (m_row * MAP_TILE_SIZE) - CHARACTER_BOUNDING_BOX_BOTTOM;
            break;
        }
    }
}

void Entity::set_action(Action action)
{
    switch (action) {
    case Action::OPEN_ARMS:
        switch (m_sprite_direction) {
        case Direction::LEFT:
            m_current_state = State::OPEN_ARMS_LEFT;
            return;
        case Direction::RIGHT:
            m_current_state = State::OPEN_ARMS_RIGHT;
            return;
        case Direction::UP:
            m_current_state = State::OPEN_ARMS_UP;
            return;
        case Direction::DOWN:
            m_current_state = State::OPEN_ARMS_DOWN;
            return;
        }
    case Action::SPEAR_ATTACK:
        switch (m_sprite_direction) {
        case Direction::LEFT:
            m_current_state = State::SPEAR_ATTACK_LEFT;
            return;
        case Direction::RIGHT:
            m_current_state = State::SPEAR_ATTACK_RIGHT;
            return;
        case Direction::UP:
            m_current_state = State::SPEAR_ATTACK_UP;
            return;
        case Direction::DOWN:
            m_current_state = State::SPEAR_ATTACK_DOWN;
            return;
        }
    case Action::WALK:
        switch (m_sprite_direction) {
        case Direction::LEFT:
            m_current_state = State::WALK_LEFT;
            return;
        case Direction::RIGHT:
            m_current_state = State::WALK_RIGHT;
            return;
        case Direction::UP:
            m_current_state = State::WALK_UP;
            return;
        case Direction::DOWN:
            m_current_state = State::WALK_DOWN;
            return;
        }
    case Action::SLASH:
        switch (m_sprite_direction) {
        case Direction::LEFT:
            m_current_state = State::SLASH_ATTACK_LEFT;
            return;
        case Direction::RIGHT:
            m_current_state = State::SLASH_ATTACK_RIGHT;
            return;
        case Direction::UP:
            m_current_state = State::SLASH_ATTACK_UP;
            return;
        case Direction::DOWN:
            m_current_state = State::SLASH_ATTACK_DOWN;
            return;
        }
    case Action::FALL:
        m_current_state = State::FALL;
        return;
    case Action::WIDE_SLASH:
        switch (m_sprite_direction) {
        case Direction::LEFT:
            m_current_state = State::WIDE_SLASH_LEFT;
            return;
        case Direction::RIGHT:
            m_current_state = State::WIDE_SLASH_RIGHT;
            return;
        case Direction::UP:
            m_current_state = State::WIDE_SLASH_UP;
            return;
        case Direction::DOWN:
            m_current_state = State::WIDE_SLASH_DOWN;
            return;
        }
    }
}

void Entity::use_skill(int skill_number)
{
    auto& skill = m_skills[skill_number];

    if (skill.mana_required() > m_mana) {
        m_game.dialogue.play_notice(Dialogue::Notice::NOT_ENOUGH_MANA);
        return;
    }

    if (skill.behavior() == Skill::OFFENSIVE && !m_attacking) {
        auto_attack();

        if (!m_attacking) {
            return;
        }
    }

    switch (skill.type()) {
    case Skill::FIRE_MAGIC_253:
        set_action(Action::OPEN_ARMS);

        m_game.dialogue.play_notice(Dialogue::Notice::FIRE_BALL_ATTACK);

        m_game.emitter.add_projectile_effect(Particle::Effect::GAS, { m_position.x + 32, m_position.y + 32 },
                                             { m_target->pos_x() + 32, m_target->pos_y() + 32 }, [this]() {
                                                 m_target->damage(100000000);
                                             });
        start_animation();
        break;
    default:
        break;
    }

    set_mana(m_mana - skill.mana_required());
}

void Entity::render(const SDL_Rect& camera)
{
    // Render the 3x3 grid surrounding the character.
    // SDL_Rect rect;

    // rect.x = ((m_position.x - camera.x) + MAP_TILE_SIZE) - MAP_TILE_SIZE / 2;
    // rect.y = ((m_position.y - camera.y) + MAP_TILE_SIZE);

    // rect.w = MAP_TILE_SIZE;
    // rect.h = MAP_TILE_SIZE;

    // SDL_SetRenderDrawBlendMode(m_game.window.renderer(), SDL_BLENDMODE_BLEND);

    // SDL_SetRenderDrawColor(m_game.window.renderer(), 255, 0, 0, 50);
    // SDL_RenderFillRect(m_game.window.renderer(), &rect);

    // for (int i = 0; i < m_attack_short_range; ++i) {
    //     for (int j = 0; j < m_attack_short_range; ++j) {
    //         rect.x = ((m_col + (i - 1)) * 32) - camera.x;
    //         rect.y = ((m_row + (j - 1)) * 32) - camera.y;
    //         rect.w = 32;
    //         rect.h = 32;

    //         SDL_SetRenderDrawColor(m_game.window.renderer(), 0, 255, 0, 50);
    //         SDL_RenderFillRect(m_game.window.renderer(), &rect);
    //     }
    // }

    // for (int i = 0; i < m_attack_long_range; ++i) {
    //     for (int j = 0; j < m_attack_long_range; ++j) {
    //         rect.x = ((m_col + (i - 1)) * 32) - camera.x;
    //         rect.y = ((m_row + (j - 1)) * 32) - camera.y;
    //         rect.w = 32;
    //         rect.h = 32;

    //         SDL_SetRenderDrawColor(m_game.window.renderer(), 0, 255, 0, 50);
    //         SDL_RenderFillRect(m_game.window.renderer(), &rect);
    //     }
    // }

    m_virtual.x = (m_position.x - camera.x);
    m_virtual.y = (m_position.z - camera.y);

    switch (m_current_state) {
    case WIDE_SLASH_UP:
    case WIDE_SLASH_DOWN:
    case WIDE_SLASH_LEFT:
    case WIDE_SLASH_RIGHT:
        m_virtual.x -= CHARACTER_WIDTH;
        m_virtual.y -= CHARACTER_HEIGHT;
        break;
    default:

        break;
    }

    // if (!m_player) {
    // m_texture->set_alpha(200);
    // }

    m_texture->render(m_virtual.x, m_virtual.y, &m_states[m_current_state][m_current_frame]);

    if (m_damage_stack.empty()) {
        return;
    }

    auto iter = m_damage_stack.begin();

    while (iter != m_damage_stack.end()) {
        auto& damage_texture = *iter;

        if (damage_texture->alpha() <= 0) {
            m_damage_stack.erase(iter);
        } else {
            // Render damage text above entity.
            const auto damage_x = m_virtual.x + (CHARACTER_HORIZONTAL_CENTER - damage_texture->width() / 2);
            const auto damage_y = m_virtual.y - ((255 - damage_texture->alpha()) / 10);

            // Render scaled by damage magnitude.
            damage_texture->render_transform(damage_x, damage_y);

            // Make damage text progressively opaque.
            // Will disappear after 17 frames (17 * 15 = 255).
            damage_texture->set_alpha(std::max(0, damage_texture->alpha() - 15));

            ++iter;
        }
    }
}

Entity::~Entity()
{
    LOG_DEBUG << "Destroying entity: " << std::endl;
}
