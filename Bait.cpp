#include "Bait.hpp"
#include <iostream>

void Bait::init(std::vector<Scene::Transform *> transform_vector)
{
    //assign_mesh_parts(transform_vector);
}

void Bait::assign_mesh_parts(std::vector<Scene::Transform *> transform_vector)
{
    for (auto t : transform_vector){
        if (t->name == "circlebait_main") {
            main_transform = t;
        }
        else if (t->name == "circlebait_main") {
            mesh = t;
        }
        else if (t->name == "circlebait_base") {
            mesh_parts.bait_base = t;
        }
        else if (t->name == "circlebait_string") {
            mesh_parts.bait_string = t;
        }

    }
}
