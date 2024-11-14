#include "Animation.hpp"
#include "Texture.hpp"

#include <algorithm>

void SlerpAnimation::update(float t)
{
    auto it = std::lower_bound(frames.begin(), frames.end(), t,
        [](const SlerpFrame& frame, float t) {
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

    *target = glm::slerp(frame1.value, frame2.value, factor);
}

Texture SpriteAnimation::get_current_frame(float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    uint32_t frame_number = uint32_t(t * float(frames.size()));
    return frames[frame_number];
}
