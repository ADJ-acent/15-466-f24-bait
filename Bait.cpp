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

    if(type_of_bait == CARROT){
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

    if(type_of_bait == CARROT){
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
            if(t->name == "carrotbait_main1") {
                carrot_bait_main_transforms.push_back(t);
            }
        }
        else if (t->name == "carrotbait_main2") {
            carrot_bait_main_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_main3") {
            carrot_bait_main_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_main4") {
            carrot_bait_main_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_main5") {
            carrot_bait_main_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_base1" || t->name == "fishbait_base") {
            mesh_parts.bait_base = t;
            if(t->name == "carrotbait_base1"){
                carrot_bait_base_transforms.push_back(t);
            }
        }
        else if (t->name == "carrotbait_base2") {
            t->enabled = false;
            carrot_bait_base_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_base3") {
            t->enabled = false;
            carrot_bait_base_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_base4") {
            t->enabled = false;
            carrot_bait_base_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_base5") {
            t->enabled = false;
            carrot_bait_base_transforms.push_back(t);
        }

        else if (t->name == "carrotbait_string1" || t->name == "fishbait_string") {
            mesh_parts.bait_string = t;
            if(t->name == "carrotbait_string1") {
                carrot_bait_string_transforms.push_back(t);
            }
        }
        else if (t->name == "carrotbait_string2") {
            t->enabled = false;
            carrot_bait_string_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_string3") {
            t->enabled = false;
            carrot_bait_string_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_string4") {
            t->enabled = false;
            carrot_bait_string_transforms.push_back(t);
        }
        else if (t->name == "carrotbait_string5") {
            t->enabled = false;
            carrot_bait_string_transforms.push_back(t);
        }
    }
}

void Bait::get_bitten(){
    bait_bites_left--;
    if(bait_bites_left > 0){
        if(type_of_bait == CARROT) {
            main_transform = carrot_bait_main_transforms[5 - bait_bites_left];
            mesh_parts.bait_base->enabled = false;
            mesh_parts.bait_base = carrot_bait_base_transforms[5 - bait_bites_left];
            mesh_parts.bait_base->enabled = true;

            std::cout << mesh_parts.bait_base->enabled << std::endl;

            mesh_parts.bait_string->enabled = false;
            mesh_parts.bait_string = carrot_bait_string_transforms[5 - bait_bites_left];
            mesh_parts.bait_string->enabled = true;
        }
        else{
            mesh_parts.bait_base->scale *= 0.8;
        }
    }
    else{
        if(type_of_bait == CARROT){
            mesh_parts.bait_base->enabled = false;
        }
        else{
            mesh_parts.bait_base->scale *= 0;
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
    std::uniform_real_distribution<float> dist_z(-10.0f, 5.0f);


    float random_x = dist_x(gen);
    float random_y = dist_y(gen);
    float random_z = dist_z(gen);

    is_active = true;
    main_transform->position = glm::vec3(random_x, random_y, random_z);

    if(type_of_bait == CARROT){
        for(auto t : carrot_bait_main_transforms){
            t->position = main_transform->position;
            t->scale = main_transform->scale;
        }
        carrot_bait_base_transforms[0]->enabled = true;
        carrot_bait_string_transforms[0]->enabled = true;
        mesh_parts.bait_base = carrot_bait_base_transforms[0];
        mesh_parts.bait_string = carrot_bait_string_transforms[0];
    }
}
