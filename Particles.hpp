#pragma once

#include "Scene.hpp"
#include "UIRenderProgram.hpp"
#include "Texture.hpp"
#include <unordered_map>

struct ParticleSystem
{
    struct Particle {
        GLuint texture;
        float total_lifetime = 0.0f;
        float current_lifetime = 0.0f;
        float square_distance_to_camera = 0.0f;
        glm::vec2 scale = glm::vec2();
        glm::vec2 base_scale = glm::vec2();
        glm::vec3 position = glm::vec3(0);
        glm::vec3 velocity = glm::vec3(0);
        glm::quat rotation= glm::quat();
        glm::vec3 tint = glm::vec3(1);
        
        enum struct Type : uint8_t {
            Bubble = 0,
        };
        // returns true if dead
        virtual bool update(float elapsed, const Scene::Camera &active_camera);
        glm::mat4 get_world_from_local() const;
        static std::shared_ptr<Particle> create(const Particle::Type type, float total_lifetime, glm::vec2 scale, glm::vec3 position, glm::vec3 velocity, glm::vec3 tint = glm::vec3(1));

        Particle() = default;
        Particle(float total_lifetime_, glm::vec2 scale_, glm::vec3 position_, glm::vec3 velocity_, glm::vec3 tint_ = glm::vec3(1)) : 
            total_lifetime(total_lifetime_), scale(scale_), base_scale(scale_), position(position_), velocity(velocity_), tint(tint_) {};

        virtual ~Particle() = default;
    };

    struct Bubble : Particle {
        // basically static... just hard coding for now
        static constexpr float water_level = 8.10915f;
        virtual bool update(float elapsed, const Scene::Camera &active_camera) override;
        Bubble(float total_lifetime_, glm::vec2 scale_, glm::vec3 position_, glm::vec3 velocity_, glm::vec3 tint_ = glm::vec3(1)) : 
            Particle(total_lifetime_, scale_, position_, velocity_, tint_) {};

        virtual ~Bubble() = default;
    };
    //many particles share the same textures, can bind the texture just once with the unordered map
    // the key is casted GLuint
    std::vector<std::shared_ptr<Particle>> particles;
    //pointer to scene's camera pointer
    Scene::Camera **active_camera;

    void add_particle(Texture texture, std::shared_ptr<Particle> particle);
    void update(float elapsed);
    void draw();
};
