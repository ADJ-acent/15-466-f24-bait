#pragma once

#include "Scene.hpp"

struct Puffer {
    Scene::Transform* main_transform = nullptr;
    Scene::Transform* camera = nullptr;
    Scene::Transform* mesh = nullptr;
    struct {
        Scene::Transform* puff_body;
        Scene::Transform* puff_l_blush;
        Scene::Transform* puff_l_eye;
        Scene::Transform* puff_l_fin;
        Scene::Transform* puff_mouth;
        Scene::Transform* puff_r_blush;
        Scene::Transform* puff_r_eye;
        Scene::Transform* puff_r_fin;
        Scene::Transform* puff_spikes;
        Scene::Transform* puff_tail;
    } mesh_parts;

    float current_pitch = 0.0f;
    float current_yaw = 0.0f;
    float build_up_time = 0.0f;
    float current_scale = 1.0f;
    float overshoot_target = 0.5f;
    float swim_cooldown = 1.0f;

    bool building_up = false;
    bool recovered = true;
    bool overshoot = false;

    glm::vec3 original_mesh_scale = glm::vec3(1.0f);
    glm::vec3 original_mesh_position = glm::vec3(0.0f);
    glm::quat original_rotation = glm::quat();
    glm::vec3 velocity = glm::vec3(0);

    inline static constexpr float puffer_scale_decay_halflife = .02f;
    inline static constexpr float puffer_scale_recover_halflife = .1f;
    inline static constexpr float puffer_velocity_halflife = .3f;
    inline static constexpr float speed = .5f;

    void init();
    void rotate_from_mouse(glm::vec2 mouse_motion);
    void start_build_up();
    void release();
    void update(glm::vec2 mouse_motion, int8_t swim_direction, float elapsed);
    void swim(int8_t swim_direction);


    glm::vec3 calculate_jitter(float elapsed);

    glm::vec3 get_forward();
    glm::vec3 get_right();

};