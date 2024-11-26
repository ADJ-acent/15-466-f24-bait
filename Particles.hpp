#pragma once

#include "Scene.hpp"
#include "UIRenderProgram.hpp"
#include "Texture.hpp"
#include <unordered_map>

struct ParticleSystem
{
    struct Particle {
        float total_lifetime = 0.0f;
        float current_lifetime = 0.0f;
        glm::vec2 scale = glm::vec2(5.0f);
        glm::vec3 position = glm::vec3(0);
        glm::vec3 velocity = glm::vec3(0);
        glm::quat rotation= glm::quat();
        glm::vec3 tint = glm::vec3(1);
        virtual void update(float elapsed, const Scene::Camera &active_camera);
        glm::mat4 get_world_from_local() const;
        
        enum struct Type : uint8_t {
            Bubble = 0,
        };
        static std::shared_ptr<Particle> create(const Particle::Type type, glm::vec3 start_position, glm::vec3 start_velocity, glm::vec3 tint);
        Particle() = default;
        Particle(glm::vec3 position_, glm::vec3 velocity_, glm::vec3 tint_) : position(position_), velocity(velocity_), tint(tint_){};
    };

    struct Bubble : Particle {
        virtual void update(float elapsed, const Scene::Camera &active_camera) override;
        Bubble(glm::vec3 position_, glm::vec3 velocity_, glm::vec3 tint_) : Particle(position_, velocity_, tint_){};
    };
    //many particles share the same textures, can bind the texture just once with the unordered map
    // the key is casted GLuint
    std::unordered_map<uint32_t, std::vector<std::shared_ptr<Particle>>> particles;
    //pointer to scene's camera pointer
    Scene::Camera **active_camera;

    void add_particle(Texture texture, std::shared_ptr<Particle> particle);
    void update(float elapsed);
    void draw();
};
