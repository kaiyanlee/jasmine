#include "characters/skill.h"

#include "characters/entity.h"

Skill::Skill(Type type, int count)
  : m_type(type)
  , m_count(count)
{
    switch (m_type) {
    case FIRE_MAGIC_251:
    case FIRE_MAGIC_252:
    case FIRE_MAGIC_253:
    case FIRE_MAGIC_254:
    case FIRE_MAGIC_255:
    case FIRE_MAGIC_256:
    case FIRE_MAGIC_257:
    case FIRE_MAGIC_258:
        m_mana_required = 100;
        m_behavior      = Behavior::OFFENSIVE;
        m_cooldown      = 1000;
    default:
        m_behavior      = Behavior::DEFENSIVE;
        m_mana_required = 0;
        m_cooldown      = 500;
    }
}

int Skill::mana_required() const
{
    return m_mana_required;
}

Skill::Behavior Skill::behavior() const
{
    return m_behavior;
}

int Skill::count() const
{
    return m_count;
}

void Skill::set_count(int count)
{
    m_count = count;
}

Skill::Type Skill::type() const
{
    return m_type;
}

bool Skill::is_cooling_down() const
{
    return m_elapsed < m_cooldown;
}

int Skill::elapsed() const
{
    return m_elapsed;
}

void Skill::set_elapsed(int elapsed)
{
    m_elapsed = elapsed;
}

int Skill::cooldown() const
{
    return m_cooldown;
}
