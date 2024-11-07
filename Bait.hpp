#pragma once

#include "Scene.hpp"
#include "Animation.hpp"
#include "Collide.hpp"

#include <iostream>
#include <vector>

enum BaitType {
    Circle,
    Square
};

struct Bait {
    Scene::Transform* main_transform = nullptr;
    Scene::Transform* mesh = nullptr;
    Collider bait_collider;
    Collider string_collider;
    BaitType type_of_bait;
    float total_life_time = 15.0f;
    float current_life_time = 0.0f;
    bool is_active = false;

    int bait_bites_left = 0;
    
    struct {
        Scene::Transform* bait_base;
        Scene::Transform* bait_string;
    } mesh_parts;

    static constexpr float eat_distance_threshold_squared = 625.0f; //25.0f

    void init(std::vector<Scene::Transform * > transform_vector, BaitType type_of_bait);

    void assign_mesh_parts(std::vector< Scene::Transform * > transform_vector);

    void random_respawn_location();

    glm::vec3 get_position();

    void reel_up(float elapsed);
};