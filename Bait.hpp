#pragma once

#include "Scene.hpp"
#include "Animation.hpp"
#include "Collide.hpp"
#include <vector>

struct Bait {
    Scene::Transform* main_transform = nullptr;
    Scene::Transform* mesh = nullptr;
    Collider bait_collider;
    Collider string_collider;
    int type_of_bait; //FIX THIS add enum
    
    struct {
        Scene::Transform* bait_base;
        Scene::Transform* bait_string;
    } mesh_parts;

    void init(std::vector<Scene::Transform * > transform_vector, int type_of_bait);

    void assign_mesh_parts(std::vector< Scene::Transform * > transform_vector);
    
    void update(float elapsed);
};