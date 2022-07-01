# Copyright (c) 2022 Kaiyan M. Lee
#
# Jasmine is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License v3.0.

cmake_minimum_required(VERSION 3.21.1 FATAL_ERROR)

project(jasmine VERSION 0.0.1 LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

# CMake project options.
# ------------------------------------------------------------------------------

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
set(BUILD_SHARED_LIBS ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

add_compile_options("-Wall")
add_compile_options("-pedantic")
add_compile_options("-fdata-sections")
add_compile_options("-ffunction-sections")
add_compile_options("-pipe")
add_compile_options("-Werror")
add_compile_options("-fvisibility=hidden")
add_compile_options("-fvisibility-inlines-hidden")
add_compile_options("-fomit-frame-pointer")
add_compile_options("-fno-semantic-interposition")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options("-O0")
else()
  add_compile_options("-O2")
endif()

add_compile_options("-Wextra")

find_program(CCACHE_PROGRAM "ccache")
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

# Executable target.
# ------------------------------------------------------------------------------

# SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)

# JSON
find_package(RapidJSON REQUIRED)

# ZLIB
find_package(ZLIB)

file(GLOB_RECURSE
     SOURCE_FILES
     "lib/*.h"
     "lib/*.cc"
     "src/*.cc"
)

add_executable(jasmine ${SOURCE_FILES})

target_include_directories(jasmine PRIVATE "lib")

# TODO: Use the actual installation directories...
target_compile_definitions(jasmine
                           PRIVATE RESOURCE_FOLDER="${CMAKE_SOURCE_DIR}/media"
                                   CONFIG_FOLDER="${CMAKE_SOURCE_DIR}/build"
                                   DATA_FOLDER="${CMAKE_SOURCE_DIR}/data"
)

target_link_libraries(jasmine
                      SDL2::Main
                      SDL2::Image
                      SDL2::TTF
                      SDL2::Mixer
                      ZLIB::ZLIB
)
