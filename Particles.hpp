#pragma once

#include "Scene.hpp"
#include "UIRenderProgram.hpp"
#include <unordered_map>

struct ParticleSystem
{
    struct Particle {
        float total_lifetime;
        float current_lifetime;
        glm::vec2 scale;
        glm::vec3 position;
        glm::vec3 velocity;
        glm::quat roation;
        virtual void update(float elapsed, const Scene::Camera &active_camera);
    };
    //many particles share the same textures, can bind the texture just once with the unordered map
    std::unordered_map<GLuint, std::vector<Particle>> particles;
    Scene::Camera &active_camera;

    void update(float elapsed);
};
