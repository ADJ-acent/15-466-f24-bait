#pragma once
#include "Bait.hpp"


struct BaitManager {
    std::vector<Bait> baits_in_use;
    std::shared_ptr<Bait> best_bait;
    int best_bait_index = 0;

    BaitManager() = default;

    void update_bait_lifetime(float elapsed);
    void bait_respawn(Scene scene, Scene const *other);
    void check_bait_in_range(glm::vec3 puffer_position, glm::vec3 puffer_view);
};