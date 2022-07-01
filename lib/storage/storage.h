// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <rapidjson/document.h>

// Storage represents a persistent JSON document.
class Storage : public rapidjson::Document
{
public:
    // Create a new storage document.
    explicit Storage();

    // Storage destructor.
    ~Storage();

public:
    // Save to disk.
    bool save();

    // Check if storage file exists.
    bool exists();

    // Load from disk.
    bool load();
};
