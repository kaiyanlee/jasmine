// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "storage/storage.h"

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>

#include <filesystem>
#include <fstream>

#include "core/common.h"
#include "core/logger.h"

Storage::Storage()
{
    SetObject();
}

bool Storage::save()
{
    std::ofstream                                stream(RESOLVE_CONFIG("storage.json"));
    rapidjson::OStreamWrapper                    osw(stream);
    rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);

    if (!Accept(writer)) {
        return false;
    }

    return true;
}

bool Storage::exists()
{
    return std::filesystem::exists(RESOLVE_CONFIG("storage.json"));
}

bool Storage::load()
{
    std::ifstream stream(RESOLVE_CONFIG("storage.json"));

    if (stream.is_open() && stream.peek() != std::ifstream::traits_type::eof()) {
        LOG_DEBUG << "Storage stream open, reading data" << std::endl;

        rapidjson::IStreamWrapper isw(stream);
        ParseStream(isw);

        LOG_DEBUG << "Finished loading storage data" << std::endl;

        return true;
    }

    return false;
}

Storage::~Storage()
{
    LOG_DEBUG << "Destroying storage" << std::endl;
}
