#pragma once
#include "Texture.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <string>

template <typename T>
struct LinearFrame { // only translation and scale
    float time;
    T value;
};

template <typename T>
struct LinearAnimation {
    std::vector<LinearFrame<T>> frames;
    T *target;

    void LinearAnimation<T>::update(float t) // set animation to current time
    {
        auto it = std::lower_bound(frames.begin(), frames.end(), t,
            [](const LinearFrame<T>& frame, float t) {
                return frame.time < t;
            });

        if (it == frames.begin()) {
            *target = it->value;
            return;
        } else if (it == frames.end()) {
            *target = (it - 1)->value;
            return;
        }

        auto& frame1 = *(it - 1);
        auto& frame2 = *it;

        float factor = (t - frame1.time) / (frame2.time - frame1.time);

        *target = glm::mix(frame1.value, frame2.value, factor);
    }

    LinearAnimation(std::vector<LinearFrame<T>> frames_, T *target_) : frames(frames_), target(target_) {};
    LinearAnimation() : target(nullptr) {};
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