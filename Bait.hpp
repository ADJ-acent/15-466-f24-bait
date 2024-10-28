#pragma once

#include "Scene.hpp"
#include "Animation.hpp"
#include <vector>

struct Bait {
    Scene::Transform* main_transform = nullptr;
    Scene::Transform* camera = nullptr;
    Scene::Transform* mesh = nullptr;
    struct {
        Scene::Transform* bait_body;
        Scene::Transform* bait_string;
    } mesh_parts;

    void init(std::vector<Scene::Transform * > transform_vector);

    void assign_mesh_parts(std::vector< Scene::Transform * > transform_vector);
    
};