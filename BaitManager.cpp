#include "BaitManager.hpp"


void BaitManager::update_bait_lifetime(float elapsed){
    for(Bait b : baits_in_use){
        if(b.is_active){
            b.current_life_time += elapsed;
        }

        if (b.current_life_time >= b.total_life_time)
        {
            b.is_active = false;
            b.reel_up(elapsed);
        }
        
    }
}

void BaitManager::bait_respawn(){
    if( best_bait_index >= 0 
    && baits_in_use[best_bait_index].bait_bites_left == 0 
    && baits_in_use[best_bait_index].is_active){
        // std::cout << "Respawn!!!" << std::endl;
        baits_unused.push_back(baits_in_use[best_bait_index]);
        baits_in_use.erase(baits_in_use.begin() + best_bait_index); 
        std::cout << baits_in_use.size() << std::endl;
        best_bait_index = -1;
    } 

    // std::cout << baits_in_use.size() << std::endl;

    if(baits_in_use.size() < 3){
        std::cout << "Respawn!!!" << std::endl;
        std::cout << baits_in_use.size() << std::endl;
        baits_unused.back().random_respawn_location();
        baits_unused.back().mesh_parts.bait_base->scale = glm::vec3(3.0f);
        baits_in_use.push_back(baits_unused.back());
        baits_unused.pop_back();
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