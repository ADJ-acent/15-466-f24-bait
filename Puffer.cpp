#include "Puffer.hpp"
#include "bezier.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>

#include <algorithm>
#include <iostream>

void Puffer::rotate_from_mouse(glm::vec2 mouse_motion)
{
    assert(transform);
    constexpr float sensitivity = 30.0f;

    float yaw_delta = -mouse_motion.x * sensitivity;
    float pitch_delta = -mouse_motion.y * sensitivity;

    // clamp the pitch and recalculate delta
    current_yaw = fmod(current_yaw + yaw_delta, 360.0f);
    current_pitch = glm::clamp(current_pitch + pitch_delta, -89.9f, 89.9f);

    glm::quat yaw_rotation = glm::angleAxis(glm::radians(current_yaw), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::quat pitch_rotation = glm::angleAxis(glm::radians(current_pitch), glm::vec3(1.0f, 0.0f, 0.0f));

    transform->rotation = yaw_rotation * pitch_rotation * original_rotation;

    transform->rotation = glm::normalize(transform->rotation);
}

void Puffer::start_build_up()
{
    assert(transform);
    if (build_up_time == 0.0f && recovered) {
        building_up = true;
        recovered = false;
    }
}

void Puffer::release()
{
    assert(transform);
    if (building_up) {
        building_up = false;
        overshoot = true;
        overshoot_target = 1.0f - (current_scale - 1.0f)/2.0f;
        velocity = get_forward() * speed * build_up_time;
    }
}

void Puffer::update(glm::vec2 mouse_motion, float elapsed)
{
    assert(transform);
    rotate_from_mouse(mouse_motion);

    {// handle movement
        float velocity_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_velocity_halflife * 2.0f));
        velocity = glm::mix(velocity, glm::vec3(0.0f), velocity_amt);
        transform->position += velocity;
        if (building_up) {
            build_up_time += elapsed * 0.5f;
            // grow
            if (build_up_time <= 1.0f) {
                // std::cout<<(1.0f+cubic_bezier(0.0f,0.5f, 0.34f, 1.0f, build_up_time))<<std::endl;
                current_scale = 1.0f + cubic_bezier(0.0f,0.8f, 0.9f, 1.0f, build_up_time);
                float shake_intensity = build_up_time*3.0f;
                glm::vec3 shake = {
                    glm::linearRand(-shake_intensity, shake_intensity),
                    glm::linearRand(-shake_intensity, shake_intensity),
                    glm::linearRand(-shake_intensity, shake_intensity)
                };
                mesh->position = original_mesh_position + shake;
            }
            // stay at peak for 1.5 seconds
            else if (build_up_time <= 1.5f) {
                // just shake
                constexpr float shake_intensity = 3.0f;
                glm::vec3 shake = {
                    glm::linearRand(-shake_intensity, shake_intensity),
                    glm::linearRand(-shake_intensity, shake_intensity),
                    glm::linearRand(-shake_intensity, shake_intensity)
                };
                mesh->position = original_mesh_position + shake;
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
    mesh->scale = original_mesh_scale * current_scale;
}

glm::vec3 Puffer::get_forward()
{
    // in blender camera faces y+, probably should fix this
    return transform->rotation * glm::vec3(0,1,0);
}

Puffer::Puffer(Scene::Transform *transform_, Scene::Transform *camera_, Scene::Transform *mesh_)
 : transform(transform_), camera(camera_), mesh(mesh_)
{
    original_mesh_scale = mesh->scale;
    original_mesh_position = mesh->position;
    original_rotation = transform->rotation;
}
