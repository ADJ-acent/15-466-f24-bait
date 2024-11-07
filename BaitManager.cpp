#include "BaitManager.hpp"


void BaitManager::update_bait_lifetime(float elapsed){
    int active_count = 0;
    for(Bait &b : baits_in_use){  
        if(b.is_active){
            b.current_life_time += elapsed;
            active_count += 1;
        }

        if (b.current_life_time >= b.total_life_time)
        {
            b.reel_up(elapsed);
        }
    }

    active_baits_num = active_count;

    if(active_baits_num < 5){
        respawn_interval_timer += elapsed;
        if(respawn_interval_timer >= 5.0f){
            bait_respawn();
            respawn_interval_timer = 0.0f;
        }
    }
}

void BaitManager::bait_respawn(){
    for(Bait &b : baits_in_use){
        if(!b.is_active){
            b.random_respawn_location();
            return;
        }
    }
} 

void BaitManager::check_bait_in_range(glm::vec3 puffer_position, glm::vec3 puffer_view){
    int bait_index = 0;
    float closest_in_view_bait = 1000.0f;

    for(Bait b : BaitManager::baits_in_use){
        glm::vec3 bait_position = b.get_position();
        glm::vec3 puff_to_bait = bait_position - puffer_position;
        float distance_squared = glm::dot(puff_to_bait, puff_to_bait);

        float cosine_angle = glm::dot(puffer_view, puff_to_bait) / glm::length(puffer_view) * glm::length(puff_to_bait);

        // bait must be in front of camera
        bool bait_in_view = puff_to_bait == glm::vec3(0) || cosine_angle > 0.707f;//0.707f is cos45, probably should store inside bait

        if(distance_squared <= Bait::eat_distance_threshold_squared && bait_in_view){
            if (distance_squared < closest_in_view_bait) {
                best_bait_index = bait_index;
                closest_in_view_bait = distance_squared;
                break;
            }
        }
        best_bait_index = -1;
        bait_index++;
    }
}