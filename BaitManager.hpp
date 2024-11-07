#pragma once
#include "Bait.hpp"


struct BaitManager {
    std::vector<Bait> baits_in_use;
    std::vector<Bait> baits_unused;
    int best_bait_index = -1;

    BaitManager() = default;

    void update_bait_lifetime(float elapsed);
    void bait_respawn(Scene scene, Scene const *other);
    void check_bait_in_range(glm::vec3 puffer_position, glm::vec3 puffer_view);
};