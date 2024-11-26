#include "Particles.hpp"
#include "ParticleProgram.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

extern Load< ParticleProgram > particle_program;

void ParticleSystem::Particle::update(float elapsed, const Scene::Camera &active_camera)
{
    // billboard the texture towards the camera
    glm::vec3 camera_position = active_camera.transform->make_local_to_world() * glm::vec4(0,0,0,1);
    glm::vec3 to_camera = glm::normalize(camera_position - position);
    glm::vec3 forward = to_camera;
    glm::vec3 up = glm::vec3(0, 0, 1);
    glm::vec3 right = glm::normalize(glm::cross(up, forward));
    glm::vec3 adjusted_up = glm::normalize(glm::cross(forward, right));
    glm::mat3 rotation_matrix(right, adjusted_up, forward);
    rotation = glm::quat_cast(rotation_matrix);
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

std::shared_ptr<ParticleSystem::Particle> ParticleSystem::Particle::create(const Particle::Type type, glm::vec3 start_position, glm::vec3 start_velocity, glm::vec3 tint)
{
    switch (type)
    {
        case Particle::Type::Bubble:
            return std::make_shared<Bubble>(start_position, start_velocity, tint);
            break;
        
        default:
            assert(false);
            break;
    }
    return std::make_shared<Particle>();
}

void ParticleSystem::add_particle(Texture texture, std::shared_ptr<Particle> particle)
{
    particles[uint32_t(texture.handle)].push_back(particle);
}

void ParticleSystem::update(float elapsed)
{
    for (auto& pair : particles) {
        std::vector<std::shared_ptr<Particle>>& particle_list = pair.second;
        for (std::shared_ptr<Particle>& particle : particle_list) {
            particle->update(elapsed,**active_camera);
        }
    }
}

void ParticleSystem::draw()
{
    assert((**active_camera).transform);
    glm::mat4 clip_from_world = (**active_camera).make_projection() * glm::mat4((**active_camera).transform->make_world_to_local());
    for (const auto& pair : particles) {
        const uint32_t texture = pair.first;
        const std::vector<std::shared_ptr<Particle>>& particle_list = pair.second;

        // bind 
        particle_program->bind_particle(texture);
        // draw
        for (const std::shared_ptr<Particle>& particle : particle_list) {
            particle_program->draw_particle(clip_from_world * particle->get_world_from_local(), particle->tint);
        }
        // unbind
        particle_program->unbind_particle();
    }
}

void ParticleSystem::Bubble::update(float elapsed, const Scene::Camera &active_camera)
{
    ParticleSystem::Particle::update(elapsed, active_camera);
}
