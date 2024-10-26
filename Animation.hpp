#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <string>

struct LinearFrame { // only translation and scale
    float time;
    glm::vec3 value;
};

struct LinearAnimation {
    std::vector<LinearFrame> frames;
    glm::vec3 get_value(float t);

    LinearAnimation(std::vector<LinearFrame> frames_) : frames(frames_){};
};