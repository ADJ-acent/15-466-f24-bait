#pragma once
#include "Texture.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>

struct LinearFrame { // only translation and scale
    float time;
    glm::vec3 value;
};

struct LinearAnimation {
    std::vector<LinearFrame> frames;
    glm::vec3 *target;

    void update(float t);

    LinearAnimation(std::vector<LinearFrame> frames_, glm::vec3 *target_) : frames(frames_), target(target_){};
    LinearAnimation() = delete;
};

struct SlerpFrame { // only translation and scale
    float time;
    glm::quat value;
};

struct SlerpAnimation {
    std::vector<SlerpFrame> frames;
    glm::quat *target;

    void update(float t);

    SlerpAnimation(std::vector<SlerpFrame> frames_, glm::quat *target_) : frames(frames_), target(target_){};
    SlerpAnimation() = delete;
};

struct SpriteAnimation {
    std::vector<Texture> frames;
    // t between 0 and 1, make sure to normalize before passing in t
    Texture get_current_frame(float t);
};