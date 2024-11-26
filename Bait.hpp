#pragma once

#include "Scene.hpp"
#include "Animation.hpp"

#include <iostream>
#include <vector>
#include <random>

enum BaitType {
    CIRCLE,
    SQUARE
};

struct Bait {
    Scene::Transform* main_transform = nullptr;

    BaitType type_of_bait;
    float total_life_time = 0.0f;
    float current_life_time = 0.0f;
    float reel_up_timer = 0.0f;
    
    bool is_active = false;
    bool currently_in_qte = false;
    int bait_bites_left = 0;

    glm::vec3 original_bait_scale;
    
    struct {
        Scene::Transform* bait_base;
        Scene::Transform* bait_string;
    } mesh_parts;

    std::vector <Scene::Transform*> carrot_bait_main_transforms;
    std::vector <Scene::Transform*> carrot_bait_base_transforms;
    std::vector <Scene::Transform*> carrot_bait_string_transforms;

    static constexpr float eat_distance_threshold_squared = 625.0f; //25.0f

    void init(std::vector<Scene::Transform * > transform_vector, BaitType type_of_bait);
    void reset();

    void assign_mesh_parts(std::vector< Scene::Transform * > transform_vector);

    void random_respawn_location();

    glm::vec3 get_position();

    void reel_up(float elapsed, float reel_up_speed = 30.0f);
    void to_siberia();
};