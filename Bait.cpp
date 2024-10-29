#include "Bait.hpp"
#include <iostream>

void Bait::init(std::vector<Scene::Transform *> transform_vector, int tob)
{
    assign_mesh_parts(transform_vector);
    type_of_bait = tob;
}

void Bait::assign_mesh_parts(std::vector<Scene::Transform *> transform_vector)
{
    for (auto t : transform_vector){
        if (t->name == "carrotbait_main" || t->name == "fishbait_main") {
            main_transform = t;
        }
        else if (t->name == "carrotbait_main" || t->name == "fishbait_main") {
            mesh = t;
        }
        else if (t->name == "carrotbait_base" || t->name == "fishbait_base") {
            mesh_parts.bait_base = t;
        }
        else if (t->name == "carrotbait_string" || t->name == "fishbait_string") {
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

glm::vec3 Bait::get_position()
{
    return main_transform->position;
}

void Bait::random_respawn_location(){
    float x_min = -50.0f, x_max = 50.0f;
    float y_min = -50.0f, y_max = 50.0f;
    float z_min = -10.0f, z_max = 10.0f;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    float random_x = x_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (x_max - x_min)));
    float random_y = y_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (y_max - y_min)));
    float random_z = z_min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (z_max - z_min)));

    main_transform->position = glm::vec3(random_x, random_y, random_z);
}
