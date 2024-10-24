#pragma once

#include "Scene.hpp"

struct Puffer {
    Scene::Transform* transform = nullptr;
    float current_pitch = 0.0f;
    float current_yaw = 0.0f;
    float build_up_time = 0.0f;
    float current_scale = 1.0f;
    float overshoot_target = 0.5f;

    Scene::Transform* camera = nullptr;
    Scene::Transform* mesh = nullptr;
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

    void rotate_from_mouse(glm::vec2 mouse_motion);
    void start_build_up();
    void release();
    void update(glm::vec2 mouse_motion, float dt);
    void swim(bool left);


    glm::vec3 calculate_jitter(float elapsed);

    glm::vec3 get_forward();
    glm::vec3 get_right();

    Puffer() = default;
    Puffer(Scene::Transform* transform_, Scene::Transform* camera_, Scene::Transform* mesh_);
};