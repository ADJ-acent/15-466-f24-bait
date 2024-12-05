#include "Bait.hpp"
#include <iostream>

extern Load< Scene > bait_scene;

void Bait::init(std::vector<Scene::Transform *> transform_vector, BaitType tob)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_lifetime(20.0f, 60.0f);

    total_life_time = dist_lifetime(gen);
    assign_mesh_parts(transform_vector);

    type_of_bait = tob;

    if(type_of_bait == CIRCLE){
        bait_bites_left = 5;
    }
    else{
        bait_bites_left = 3;
    }

    main_transform->scale = glm::vec3(3.0f);
    original_bait_scale = mesh_parts.bait_base->scale;
}

void Bait::reset(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_lifetime(20.0f, 60.0f);

    current_life_time = 0.0f;
    total_life_time = dist_lifetime(gen);
    reel_up_timer = 0.0f;

    if(type_of_bait == CIRCLE){
        bait_bites_left = 5;
    }
    else{
        bait_bites_left = 3;
    }

    mesh_parts.bait_base->scale = original_bait_scale;
}

void Bait::assign_mesh_parts(std::vector<Scene::Transform *> transform_vector)
{
    for (auto t : transform_vector){
        if (t->name == "carrotbait_main1" || t->name == "fishbait_main") {
            main_transform = t;
            carrot_bait_main_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_base1" || t->name == "fishbait_base") {
            mesh_parts.bait_base = t;
            carrot_bait_base_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_string1" || t->name == "fishbait_string") {
            mesh_parts.bait_string = t;
            carrot_bait_string_transforms.push_back(t);
        }

    }
}



void Bait::reel_up(float elapsed, float reel_up_speed)
{
    assert(main_transform);

    is_active = false;
    reel_up_timer += elapsed;
    if(reel_up_timer <= 3.0f){
        main_transform->position.z += reel_up_speed * elapsed;
    }
    else{
        to_siberia();
    }
}

void Bait::to_siberia(){
    assert(main_transform);
    main_transform->position = glm::vec3(5000.0f, 0.0f, 0.0f);
    reset();
}

glm::vec3 Bait::get_position()
{
    return main_transform->position;
}

void Bait::random_respawn_location(){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_x(-120.0f, 120.0f);
    std::uniform_real_distribution<float> dist_y(-120.0f, 120.0f);
    std::uniform_real_distribution<float> dist_z(-15.0f, 5.0f);


    float random_x = dist_x(gen);
    float random_y = dist_y(gen);
    float random_z = dist_z(gen);

    is_active = true;
    main_transform->position = glm::vec3(random_x, random_y, random_z);
}
