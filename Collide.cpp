#include "Collide.hpp"

Collider calculate_collider(Scene::Transform *transform, const Mesh &mesh) {
    glm::vec3 local_min = mesh.min;
    glm::vec3 local_max = mesh.max;

    glm::vec3 world_min = glm::vec3(transform->make_local_to_world() * glm::vec4(local_min, 1.0f));
    glm::vec3 world_max = glm::vec3(transform->make_local_to_world() * glm::vec4(local_max, 1.0f));

    return Collider(world_min, world_max);
}