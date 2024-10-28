#include "Bait.hpp"
#include <iostream>

void Bait::init(std::vector<Scene::Transform *> transform_vector)
{
    assign_mesh_parts(transform_vector);
}

void Bait::assign_mesh_parts(std::vector<Scene::Transform *> transform_vector)
{
    for (auto t : transform_vector){
        if (t->name == "circlebait_main" || t->name == "squarebait_main") {
            main_transform = t;
        }
        else if (t->name == "circlebait_main" || t->name == "squarebait_main") {
            mesh = t;
        }
        else if (t->name == "circlebait_base" || t->name == "squarebait_base") {
            mesh_parts.bait_base = t;
        }
        else if (t->name == "circlebait_string" || t->name == "squarebait_string") {
            mesh_parts.bait_string = t;
        }

    }
}

void Bait::update(float elapsed)
{
    assert(main_transform);
    float velocity_amt = 1.0f;
    main_transform->position += velocity_amt*elapsed;
}
