#include "Particles.hpp"
#include "ParticleProgram.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <algorithm>


extern Load< ParticleProgram > particle_program;

bool ParticleSystem::Particle::update(float elapsed, const Scene::Camera &active_camera)
{
    {// billboard the texture towards the camera
        glm::vec3 camera_position = active_camera.transform->make_local_to_world() * glm::vec4(0,0,0,1);
        glm::vec3 to_camera_unnormalized = camera_position - position;
        glm::vec3 to_camera = glm::normalize(to_camera_unnormalized);
        glm::vec3 forward = to_camera;
        glm::vec3 up = glm::vec3(0, 0, 1);
        glm::vec3 right = glm::normalize(glm::cross(up, forward));
        glm::vec3 adjusted_up = glm::normalize(glm::cross(forward, right));
        glm::mat3 rotation_matrix(right, adjusted_up, forward);
        rotation = glm::quat_cast(rotation_matrix);
        square_distance_to_camera = glm::dot(to_camera_unnormalized, to_camera_unnormalized);
    }

    {//lifetime update
        current_lifetime += elapsed;
        if (current_lifetime >= total_lifetime) {
            return true;
        }
    }

    return false;
}

glm::mat4 ParticleSystem::Particle::get_world_from_local() const
{
    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));

    // Create rotation matrix from quaternion
    glm::mat4 rotation_matrix = glm::toMat4(rotation);

    // Create translation matrix
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position);

    return translation_matrix * rotation_matrix * scale_matrix;
}

std::shared_ptr<ParticleSystem::Particle> ParticleSystem::Particle::create(const Particle::Type type, float total_lifetime, glm::vec2 scale, glm::vec3 position, glm::vec3 velocity, glm::vec3 tint)
{
    switch (type)
    {
        case Particle::Type::Bubble:
            return std::make_shared<Bubble>(total_lifetime, scale, position, velocity, tint);
            break;
        
        default:
            assert(false);
            break;
    }
    // never reaches here, just preventing compiler from complaining
    return std::make_shared<Particle>();
}

void ParticleSystem::add_particle(Texture texture, std::shared_ptr<Particle> particle)
{
    particle->texture = texture.handle;
    particles.push_back(particle);
}

void ParticleSystem::update(float elapsed)
{
    std::sort(particles.begin(), particles.end(), [](const std::shared_ptr<ParticleSystem::Particle> a, const std::shared_ptr<ParticleSystem::Particle> b) {
        return a->square_distance_to_camera < b->square_distance_to_camera;
    });
    for (auto it = particles.begin(); it != particles.end(); ) {
    if ((*it)->update(elapsed, **active_camera)) 
    {
        it = particles.erase(it); // no longer alive
    } 
    else {
        ++it;
    }
}
}

void ParticleSystem::draw()
{
    assert((**active_camera).transform);
    glm::mat4 clip_from_world = (**active_camera).make_projection() * glm::mat4((**active_camera).transform->make_world_to_local());
    for (const auto& particle : particles) {
        // bind 
        particle_program->bind_particle();
        // draw
        particle_program->draw_particle(clip_from_world * particle->get_world_from_local(), particle->texture, particle->tint);
        // unbind
        particle_program->unbind_particle();
    }
}

bool ParticleSystem::Bubble::update(float elapsed, const Scene::Camera &active_camera)
{
    bool dead = ParticleSystem::Particle::update(elapsed, active_camera);
    scale = base_scale * (total_lifetime - current_lifetime) / total_lifetime;
    position += velocity * elapsed;
    position.z += elapsed * 3.0f; // upward force
    dead |= position.z >= water_level - scale.y / 2.0f; // get rid of bubble if above water
    float velocity_amt = 1.0f - std::pow(0.5f, elapsed / (1.0f * 2.0f));
    velocity = glm::mix(velocity, glm::vec3(0), velocity_amt);

    return dead;
}
