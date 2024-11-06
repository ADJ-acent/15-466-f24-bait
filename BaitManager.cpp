#include "BaitManager.hpp"


void BaitManager::update_bait_lifetime(float elapsed){
    for(Bait b : baits_in_use){
        if(b.is_active){
            b.current_life_time += elapsed;
        }
    }
}

void BaitManager::bait_respawn(Scene scene, Scene const *other){
    // std::cout << (best_bait == nullptr) << std::endl;
    // if(best_bait != nullptr){
    //     std::cout << best_bait->bait_bites_left << std::endl;
    // }
    

    if( best_bait!= nullptr && best_bait->bait_bites_left == 0){

        std::cout << "Hello" << std::endl;
        // best_bait = nullptr;
        // baits_in_use.pop_back();

        // Bait new_bait = Bait();
        // //pick either square or circle
        // std::srand(static_cast<unsigned int>(std::time(nullptr)));
        // auto circle_or_square = static_cast<BaitType>(std::rand() % 2);
        // std::vector<Scene::Transform *> new_bait_transforms;
        // if(circle_or_square==0){
        //     new_bait_transforms = scene.spawn(*other, CARROT_BAIT);
        // } else {
        //     new_bait_transforms = scene.spawn(*other, FISH_BAIT);
        // }
        // new_bait.init(new_bait_transforms, circle_or_square);
        // BaitManager::baits_in_use.push_back(new_bait);
        // new_bait.random_respawn_location();
    } 
}

void BaitManager::check_bait_in_range(glm::vec3 puffer_position, glm::vec3 puffer_view){
    best_bait_index = 0;
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
                best_bait = std::make_shared< Bait >(b);
                closest_in_view_bait = distance_squared;
                break;
            }
        }

        best_bait_index++;
    }
}