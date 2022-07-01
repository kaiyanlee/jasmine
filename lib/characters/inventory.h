// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

// Inventory represents a store of gear and/or valuables.
class Inventory
{
public:
    // Create a new inventory.
    explicit Inventory();

public:
    // Add some gold to the inventory.
    void add_gold(int gold_amount);

    // Take away some gold.
    void remove_gold(int gold_amount);

    // Return the amount of gold stored.
    int gold() const;

private:
    int m_gold;
};
