#include "Scene.hpp"
#include "Mesh.hpp"

#include <glm/glm.hpp>
    
struct Collider {
    glm::vec3 min;
    glm::vec3 max;

    Collider() : min(glm::vec3(std::numeric_limits<float>::infinity())), max(glm::vec3(-std::numeric_limits<float>::infinity())) {}
    Collider(glm::vec3 min_, glm::vec3 max_) : min(min_), max(max_) {}

    bool collides(const Collider& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
            (min.y <= other.max.y && max.y >= other.min.y) &&
            (min.z <= other.max.z && max.z >= other.min.z);
    }
};

Collider calculate_collider(Scene::Transform* object, const Mesh &mesh);