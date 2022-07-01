// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#include "core/game.h"
#include "core/logger.h"

int main(int argc, char** argv)
{
    int frame_rate = 30;

    while (argc > 1) {
        if (const auto a = argv[--argc]; strcmp(a, "--help") == 0 || strcmp(a, "-h") == 0) {
            puts("");
            puts("Summary:");
            puts("");
            puts("  Jasmine - 0.0.1");
            puts("");
            puts("  This software is a single player action RPG.");
            puts("");
            puts("Usage:");
            puts("");
            puts("  jasmine [options]");
            puts("");
            puts("Options:");
            puts("");
            puts("  -h or --help       Print this help message.");
            puts("  -v or --version    Print the project version number.");
            puts("");
            puts("  --slow             Render at 15 fps.");
            puts("  --fast             Render at 60 fps.");
            puts("");
            return EXIT_SUCCESS;
        } else if (strcmp(a, "--version") == 0 || strcmp(a, "-v") == 0) {
            puts("0.0.1");
            return EXIT_SUCCESS;
        } else if (strcmp(a, "--slow") == 0) {
            frame_rate = 15;
        } else if (strcmp(a, "--fast") == 0) {
            frame_rate = 60;
        } else {
            printf("Unknown command-line option: %s\n", a);
            return EXIT_FAILURE;
        }
    }

    Game game;
    game.set_frame_rate(frame_rate);
    game.set_app_name("Jasmine");

    if (!game.initialize()) {
        LOG_ERROR << "Failed to initialize game object. Exiting..." << std::endl;
        return EXIT_FAILURE;
    }

    return game.start();
}
