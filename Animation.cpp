#include "Animation.hpp"

void LinearAnimation::update(float t) // set animation to current time
{
    auto it = std::lower_bound(frames.begin(), frames.end(), t,
        [](const LinearFrame& frame, float t) {
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
