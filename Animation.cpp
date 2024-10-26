#include "Animation.hpp"

glm::vec3 LinearAnimation::get_value(float t)
{
    auto it = std::lower_bound(frames.begin(), frames.end(), t,
        [](const LinearFrame& frame, float t) {
            return frame.time < t;
    });

    if (it == frames.begin()) {
        return it->value;
    } else if (it == frames.end()) {
        return (it - 1)->value;
    }
    auto& frame1 = *(it - 1);
    auto& frame2 = *it;

    float factor = (t - frame1.time) / (frame2.time - frame1.time);

    return glm::mix(frame1.value, frame2.value, factor);
}
