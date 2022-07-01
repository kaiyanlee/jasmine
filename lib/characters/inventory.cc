#include "characters/inventory.h"

Inventory::Inventory()
  : m_gold(0)
{
}

void Inventory::add_gold(int gold_amount)
{
    m_gold += gold_amount;
}

void Inventory::remove_gold(int gold_amount)
{
    m_gold -= gold_amount;
}

int Inventory::gold() const
{
    return m_gold;
}
