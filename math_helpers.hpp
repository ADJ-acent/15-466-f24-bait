#pragma once
#include <cmath>

inline float cubic_bezier(float P0, float P1, float P2, float P3, float t) {
    float one_minus_t = 1.0f - t;

    // Using the cubic Bezier formula for 1D
    float result = (P0 * std::pow(one_minus_t, 3.0f)) +
                   (P1 * (3.0f * std::pow(one_minus_t, 2.0f) * t)) +
                   (P2 * (3.0f * one_minus_t * std::pow(t, 2.0f))) +
                   (P3 * std::pow(t, 3.0f));

    return result;
}