#pragma once

#include "Scene.hpp"
#include "Sound.hpp"
#include "Animation.hpp"
#include "CollisionDetection.hpp"
#include "CollisionDetection.hpp"
#include <vector>
#include <array>

struct Bait;
struct ParticleSystem;

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

    //store bools of if collectibles are collected
    struct {
        bool anchor = false;
        bool bucket = false;
        bool beachball = false;
        bool popsicle = false;
        bool treasure = false;
    } collectibles;

    std::vector<LinearAnimation<glm::vec3>> build_up_animations;
    std::vector<SlerpAnimation> swim_animation;

    //sound effects
    std::shared_ptr< Sound::PlayingSample > flipper_sound;
    std::shared_ptr< Sound::PlayingSample > through_water_sound;
    std::shared_ptr< Sound::PlayingSample > blow_up_sound;
    std::shared_ptr< Sound::PlayingSample > whoosh_sound;
    std::shared_ptr< Sound::PlayingSample > bump_1_sound;
    std::shared_ptr< Sound::PlayingSample > congrats_sound;

    float current_pitch = 0.0f;
    float current_yaw = 0.0f;
    float build_up_time = 0.0f;
    float current_scale = 1.0f;
    float overshoot_target = 0.5f;
    float swim_cooldown = 0.0f;
    float idletime = 0.0f;
    float release_rotate_angle = 0.0f;
    float total_release_angle = 0.0f;
    float default_spring_arm_length;
    float oxygen_level = 200.0f;
    float bubble_spawn_cooldown = 0.0f;
    uint8_t swimming_side = 0; // 0 is left, 1 is right
    uint8_t last_swimming_size = 2;

    bool building_up = false;
    bool recovered = true;
    bool overshoot = false;
    bool above_water = false;
    bool in_menu = false;
    bool whoosh_sound_played = true;
    bool in_qte = false;
    bool reeled_up = false;

    glm::vec3 original_mesh_scale = glm::vec3(1.0f);
    glm::vec3 original_mesh_position = glm::vec3(0.0f);
    glm::vec3 spring_arm_normalized_displacement;
    glm::quat original_mesh_rotation = glm::quat();
    glm::quat original_rotation = glm::quat();
    glm::quat base_rotation = glm::quat();
    glm::vec3 velocity = glm::vec3(0);
    glm::vec3 pause_velocity = glm::vec3(0);
    glm::vec3 release_rotate_axis = glm::vec3(0);
    Scene *scene;

    CollisionDetector puffer_collider;
    ParticleSystem *particle_system = nullptr;

    inline static constexpr float puffer_scale_decay_halflife = .02f;
    inline static constexpr float puffer_scale_recover_halflife = .1f;
    inline static constexpr float puffer_velocity_halflife = .3f;
    inline static constexpr float puffer_rotation_return_halflife = 0.1f;
    inline static constexpr float puffer_rotation_release_halflife = .3f;
    inline static constexpr float speed = 1.0f;
    inline static constexpr float gravity = 1.0f;
    inline static constexpr float oxygen_down_speed = 3.0f;
    inline static constexpr float oxygen_up_speed = 10.0f;


    std::vector<std::string> names = {"PuffMain", "PuffMesh", "PuffCam", "PuffBody", "PuffLBlush", "PuffLEye", "PuffLFin", "PuffMouth", "PuffRBlush", "PuffREye", "PuffRFin", "PuffSpikes", "PuffTail"};
    std::vector<Scene::Transform *> collected = {};

    void init(std::vector<Scene::Transform * > transform_vector, Scene *scene, ParticleSystem *particle_system);
    void rotate_from_mouse(glm::vec2 mouse_motion);
    void recalibrate_rotation();
    void start_build_up();
    void release();
    void update(glm::vec2 mouse_motion, int8_t swim_direction, float elapsed);
    void handle_collision(std::array<glm::vec3,2> collision_point,float bounce_factor);
    void update_build_up_animations(float t);
    void swim(int8_t swim_direction);
    void switch_to_main_menu_camera();
    void switch_to_default_camera();
    void check_collectibles(Scene::Transform* collided_object);
    void spawn_bubbles(uint32_t count);


    void assign_mesh_parts(std::vector< Scene::Transform * > transform_vector);


    glm::vec3 calculate_jitter(float elapsed);

    glm::vec3 get_forward();
    glm::vec3 get_right();
    glm::vec3 get_position();

    void qte_enter(Bait *bait);
    void qte_exit();
    void qte_death(Bait *bait);
};