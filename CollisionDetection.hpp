#pragma once
#include <glm/glm.hpp>
#include "Scene.hpp"
#include "Mesh.hpp"
#include "Puffer.hpp"

struct CollisionDetector{
    struct AABB{
        glm::vec3 min;
        glm::vec3 max;
    };

    struct OBB{
    };


    // glm::vec3 center;
    AABB aabb = {glm::vec3(0),glm::vec3(0)};
    Scene::Transform *transform;
    Puffer *puffer;

    //for min and max, calculate the AABB
    void calculate_AABB(Scene::Transform* transform, const Mesh &mesh);

    //init a collision detector for an object
    void init(Puffer &puffer, Scene::Transform *t, const Mesh &m);

    //check collision given the collision detector and another mesh and transform
    bool check_collision(const Scene::Transform *transform_other, const Mesh *other_mesh, const MeshBuffer *meshbuffer);
};
