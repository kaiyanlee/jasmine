// Copyright (c) 2022 Kaiyan M. Lee
//
// Jasmine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License v3.0.

#pragma once

#include <cmath>

template<typename T>
struct Vector2D
{
    T x;
    T y;
};

template<typename T>
struct Vector3D
{
    T x;
    T y;
    T z;
};

template<typename T>
inline void normalize(Vector2D<T>& vec)
{
    const auto hyp = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    vec.x /= hyp;
    vec.y /= hyp;
}

inline float linear_interpolation(float start, float end, float t)
{
    return start + (end - start) * t;
}
