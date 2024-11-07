#pragma once
#include "Bait.hpp"


struct BaitManager {
    std::vector<Bait> baits_in_use;
    std::vector<Bait> baits_unused;
    int best_bait_index = -1;
    int active_baits_num;

    float respawn_interval_timer = 0.0f;

    BaitManager() = default;

    void update_bait_lifetime(float elapsed);
    void bait_respawn();
    void check_bait_in_range(glm::vec3 puffer_position, glm::vec3 puffer_view);
};