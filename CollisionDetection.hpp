#pragma once
#include "Scene.hpp"
#include "Mesh.hpp"
#include <glm/glm.hpp>
#include <array>

struct Puffer;

struct CollisionDetector{
    struct AABB{
        glm::vec3 min;
        glm::vec3 max;
    };

    // glm::vec3 center;
    AABB aabb = {glm::vec3(0),glm::vec3(0)};
    Scene::Transform *transform;
    Puffer *puffer;

    //for min and max, calculate the AABB
    void calculate_AABB(const Mesh &mesh);

    //init a collision detector for an object
    void init(Puffer *puffer, Scene::Transform *t, const Mesh &m);

    //check collision given the collision detector and another mesh and transform
    std::array<glm::vec3,2> check_puffer_collision(const Scene::Transform *transform_other, const Mesh *other_mesh, std::array<glm::vec3, 2> closest_collision_point);

    bool check_ray_mesh_collision(const glm::vec3 p0, const glm::vec3 dir, const Scene::Transform *transform_other, const Mesh *other_mesh, float& t);

    bool check_over_water(const Scene::Transform *water_transform, const Mesh *water_mesh);

    static bool line_triangle_intersection(const glm::vec3& P0, const glm::vec3& dir,
        const glm::vec3& V0, const glm::vec3& V1, const glm::vec3& V2, float& t);
};
