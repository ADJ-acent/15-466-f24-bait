#include "Puffer.hpp"
#include "math_helpers.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp> 
#include <algorithm>
#include <iostream>

void Puffer::init()
{
    original_mesh_scale = mesh->scale;
    original_mesh_position = mesh->position;
    original_mesh_rotation = mesh->rotation;
    base_rotation = original_mesh_rotation;
    original_rotation = main_transform->rotation;

    { //set up build up animations
        build_up_animations.reserve(10);
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(1.0f)},
                LinearFrame{1.0f, glm::vec3(2.0f)},
            }, 
            &mesh_parts.puff_body->scale)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(-3.1608f, -0.18379f, 2.8906f)},
                LinearFrame{1.0f, glm::vec3(-5.3108f, -0.18379f, 7.19066f)},
            }, 
            &mesh_parts.puff_l_blush->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(-3.1889f, 0.16394f, 2.919f)},
                LinearFrame{1.0f, glm::vec3(-5.3389f, 0.16394f, 7.204f)},
            }, 
            &mesh_parts.puff_l_eye->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(-3.9862f,-0.005412f, -0.30636f)},
                LinearFrame{1.0f, glm::vec3(-7.4861f, -0.005412f, -3.3064f)},
            }, 
            &mesh_parts.puff_l_fin->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(0.0f,0.0f, 4.8114f)},
                LinearFrame{1.0f, glm::vec3(0.0f,0.0f, 9.4984f)},
            }, 
            &mesh_parts.puff_mouth->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(1.0f)},
                LinearFrame{1.0f, glm::vec3(0.6f)},
            }, 
            &mesh_parts.puff_mouth->scale)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(3.1608f, -0.18379f, 2.8906f)},
                LinearFrame{1.0f, glm::vec3(5.3108f, -0.18379f, 7.19066f)},
            }, 
            &mesh_parts.puff_r_blush->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(3.1889f, 0.16394f, 2.919f)},
                LinearFrame{1.0f, glm::vec3(5.3389f, 0.16394f, 7.204f)},
            }, 
            &mesh_parts.puff_r_eye->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(3.9862f,-0.005412f, -0.30636f)},
                LinearFrame{1.0f, glm::vec3(7.4861f, -0.005412f, -3.3064f)},
            }, 
            &mesh_parts.puff_r_fin->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(1.0f)},
                LinearFrame{1.0f, glm::vec3(2.0f)},
            }, 
            &mesh_parts.puff_spikes->scale)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame{0.0f, glm::vec3(-0.02516f, -0.045457f, -4.4023f)},
                LinearFrame{1.0f, glm::vec3(-0.02516f, -0.045457f, -8.9023f)},
            }, 
            &mesh_parts.puff_tail->position)
        );
    }
}

void Puffer::rotate_from_mouse(glm::vec2 mouse_motion)
{
    assert(main_transform);
    if (mouse_motion == glm::vec2(0.0f)) return;
    constexpr float sensitivity = 30.0f;

    float yaw_delta = -mouse_motion.x * sensitivity;
    float pitch_delta = mouse_motion.y * sensitivity;

    // clamp the pitch and recalculate delta
    current_yaw = fmod(current_yaw + yaw_delta, 360.0f);
    current_pitch = glm::clamp(current_pitch + pitch_delta, -89.9f, 89.9f);

    glm::quat yaw_rotation = glm::angleAxis(glm::radians(current_yaw), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::quat pitch_rotation = glm::angleAxis(glm::radians(current_pitch), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::quat total_rotation = yaw_rotation * pitch_rotation;
    glm::quat old_rotation = main_transform->rotation;
    main_transform->rotation =  glm::normalize(total_rotation * original_rotation);
    glm::quat new_to_old = glm::inverse(main_transform->rotation) * old_rotation;

    // keep mesh stationary in world position
    base_rotation*= new_to_old;
    if (!building_up) { // when building up, orient the fish tail to the screen
        mesh->rotation = base_rotation;
    }
}

void Puffer::start_build_up()
{
    assert(main_transform);
    if (build_up_time == 0.0f && recovered) {
        building_up = true;
        recovered = false;
    }
}

void Puffer::release()
{
    assert(main_transform);
    if (building_up) {
        building_up = false;
        overshoot = true;
        overshoot_target = 1.0f - (current_scale - 1.0f)/2.0f;
        velocity = get_forward() * speed * build_up_time;
        mesh->position = original_mesh_position;
        release_rotate_angle = 20.0f * build_up_time;
        total_release_angle = 0.0f;
        release_rotate_axis = glm::normalize(glm::linearRand(glm::vec3(-1.0f), glm::vec3(1.0f)));
    }
}

void Puffer::update(glm::vec2 mouse_motion, int8_t swim_direction, float elapsed)
{
    assert(main_transform);

    rotate_from_mouse(mouse_motion);

    constexpr float swim_cooldown_threshold = 0.6f;

    if (swim_cooldown == 0.0f) {
        if (swim_direction != 0) {
            swim(swim_direction);
            swim_cooldown = 0.001f; // increment slightly to start the timer
        }
    }
    else {
        swim_cooldown += elapsed;
        if (swim_cooldown > swim_cooldown_threshold) {
            swim_cooldown = 0.0f;
        }
    }

    {// handle movement
        float velocity_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_velocity_halflife * 2.0f));
        velocity = glm::mix(velocity, glm::vec3(0.0f), velocity_amt);
        main_transform->position += velocity;
        if (building_up) {
            build_up_time += elapsed * 0.5f;
            // grow
            if (build_up_time <= 1.0f) {
                // std::cout<<(1.0f+cubic_bezier(0.0f,0.5f, 0.34f, 1.0f, build_up_time))<<std::endl;
                current_scale = 1.0f + cubic_bezier(0.0f,0.8f, 0.9f, 1.0f, build_up_time);
                mesh->position = original_mesh_position + calculate_jitter(elapsed);
            }
            // stay at peak for 1.5 seconds
            else if (build_up_time <= 1.5f) {
                // just shake
                mesh->position = original_mesh_position + calculate_jitter(elapsed);
            }
            // shrink
            else {
                mesh->position = original_mesh_position;
                release();
            }
        }
        // shrink phase
        else if (!recovered) {
            //recover from overshoot
            if (current_scale < 1.0f && !overshoot) {
                float amt = 1.0f - std::pow(0.5f, elapsed / (puffer_scale_recover_halflife * 2.0f));
                current_scale = glm::mix(current_scale, 1.1f, amt);
                //fully recovered
                if (current_scale > 0.99f) {
                    recovered = true;
                    current_scale = 1.0f;
                    build_up_time = 0.0f;
                }
            }
            else {
                float amt = 1.0f - std::pow(0.5f, elapsed / (puffer_scale_decay_halflife * 2.0f));
                
                current_scale = glm::mix(current_scale, overshoot_target, amt);
                if (current_scale < overshoot_target + 0.01f) {
                    overshoot = false;
                }
            }
        }
    }

    {// update animation based on scale of the pufferfish
        update_build_up_animations(current_scale-1.0f);
        if (current_scale < 1.0f) {
            mesh->scale = original_mesh_scale * current_scale;
        }
    }

    {// mesh rotation
        if (release_rotate_angle > 1.0f) {
            float rotation_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_rotation_release_halflife * 2.0f));
            if (building_up) { // experimental...conflicted on how this feels
                mesh->rotation = glm::slerp(mesh->rotation, original_mesh_rotation, rotation_amt);
            }
            else {
                total_release_angle += elapsed * release_rotate_angle;
                mesh->rotation = base_rotation * glm::angleAxis(total_release_angle, release_rotate_axis);
                release_rotate_angle = glm::mix(release_rotate_angle, 0.0f, rotation_amt);
            }
        }
        else {//only return to tail view when we aren't rolling
            // update mesh rotation to return to normal (if we rotated camera recently)
            float rotation_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_rotation_return_halflife * 2.0f));
            mesh->rotation = glm::slerp(mesh->rotation, original_mesh_rotation, rotation_amt);
            total_release_angle = 0.0f;
            base_rotation = mesh->rotation;
        }
    }

}

void Puffer::update_build_up_animations(float t)
{
    for (LinearAnimation& animation : build_up_animations) {
        animation.update(t);
    }
}

void Puffer::swim(int8_t swim_direction)
{
    float build_up_penaulty = 1.0f / current_scale;

    velocity += get_forward() * (0.15f * build_up_penaulty) + (float(swim_direction) * 0.03f * build_up_penaulty) * get_right();

}

glm::vec3 Puffer::calculate_jitter(float elapsed)
{
    static float time = 0.0f;
    time += elapsed;
    time = fmodf(time, 10.0f);
    constexpr float total_amplitude = .5f;
    float shake_intensity;
    if (build_up_time < 1.0f) {
        shake_intensity = build_up_time * total_amplitude;
    }
    else {
        shake_intensity = total_amplitude;
    }
    // Generate Perlin noise for each axis, adding multiple frequencies
    glm::vec3 shake = glm::vec3(0.0f);
    float frequency = 10.0f;
    float amplitude = 20.0f;

    float shake_1 = glm::perlin(glm::vec3(time) * frequency * amplitude);
    shake += glm::vec3(-shake_1, -shake_1, shake_1);

    amplitude *= 0.5f;
    frequency *= 2.0f;
    float shake_2 = glm::perlin(glm::vec3(time+10.0f) * frequency * amplitude);
    shake += glm::vec3(shake_2, -shake_2, -shake_2);

    amplitude *= 0.5f;
    frequency *= 2.0f;
    float shake_3 = glm::perlin(glm::vec3(time+20.0f) * frequency * amplitude);
    shake += glm::vec3(-shake_3, shake_3, -shake_3);

    // Normalize the shake so it doesn't exceed the expected intensity
    shake *= shake_intensity;  // Apply the shake intensity
    return shake;
}

glm::vec3 Puffer::get_forward()
{
    return main_transform->rotation * glm::vec3(0,0,1);
}

glm::vec3 Puffer::get_right()
{
    return main_transform->rotation * glm::vec3(-1,0,0);
}