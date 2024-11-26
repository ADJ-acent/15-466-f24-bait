#include "CollisionDetection.hpp"
#include "Collide.hpp"
#include "Puffer.hpp"
#include "iostream"
#include <glm/gtx/norm.hpp>

//project pt to the plane of triangle a,b,c and return the barycentric weights of the projected point:
glm::vec3 barycentric_weights(glm::vec3 const &a, glm::vec3 const &b, glm::vec3 const &c, glm::vec3 const &pt) {
	// CREDIT: Code from class
	// https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
    // From Christer Ericson's Real time Collision Detection
	
    
    glm::vec3 v0 = b-a, v1 = c-a, v2 = pt-a;
    
    float d00 = glm::dot(v0,v0);
    float d01 = glm::dot(v0,v1);
    float d11 = glm::dot(v1,v1);
    float d20 = glm::dot(v2,v0);
    float d21 = glm::dot(v2,v1);
    
    float denominator = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / denominator;
    float w = (d00 * d21 - d01 * d20) / denominator;
    
    return glm::vec3(1.0f-v-w,v,w);
}

void CollisionDetector::calculate_AABB(const Mesh &mesh)
{
    glm::vec3 local_min = mesh.min;
    glm::vec3 local_max = mesh.max;

    glm::vec3 world_min = glm::vec3(transform->make_local_to_world() * glm::vec4(local_min, 1.0f));
    glm::vec3 world_max = glm::vec3(transform->make_local_to_world() * glm::vec4(local_max, 1.0f));

    aabb.max = world_max;
    aabb.min = world_min;
}

void CollisionDetector::init(Puffer *p, Scene::Transform *t, const Mesh &m)
{
    //get radius
    transform = t; //store pointer to transform
    puffer = p;

    calculate_AABB(m);

}

std::array<glm::vec3,2> CollisionDetector::check_puffer_collision(const Scene::Transform *transform_other, const Mesh *other_mesh, std::array<glm::vec3, 2> closest_collision_point)
{
    // find barycentric coordinates of center of sphere on the triangle 
    // then checking distance
    // if lesser than radius then colliding

    //recalculate center and positions given current position
    // calculate_AABB(transform,mesh);
    glm::vec3 center = transform->position;
    assert(other_mesh);

    // glm::vec3 closest = glm::vec3(std::numeric_limits<float>::infinity());
    glm::vec3 closest = closest_collision_point[0];
    // glm::vec3 closest_point_normal = glm::vec3(0);
    glm::vec3 closest_point_normal = closest_collision_point[1];
    float closest_dis2 = glm::length2(center - closest); //starts with the
	// float closest_dis2 = std::numeric_limits< float >::infinity();
    float radius = puffer->current_scale*4.5f;

    auto get_world_normal = [&](Triangle triangle, glm::vec3 barycentric_coord, glm::mat4x4 world_from_local) {
        glm::mat3 world_from_local_normal = glm::mat3(glm::transpose(glm::inverse(world_from_local)));
        const glm::vec3 a_normal = world_from_local_normal * triangle.a_normal;
        const glm::vec3 b_normal = world_from_local_normal * triangle.b_normal;
        const glm::vec3 c_normal = world_from_local_normal * triangle.c_normal;

        return barycentric_coord.x * a_normal + barycentric_coord.y * b_normal + barycentric_coord.z * c_normal;
    };
    glm::mat4x4 world_from_local = transform_other->make_local_to_world();
    // make puffer aabb
    const glm::vec3 puffer_min = center - glm::vec3(radius);
    const glm::vec3 puffer_max = center + glm::vec3(radius);

    if (test_mesh_obb_overlap(puffer_min, puffer_max, glm::mat4(1), other_mesh->min, other_mesh->max, world_from_local)) {
        for (Triangle t : other_mesh->triangles) {
            //find closest point on triangle:
            glm::vec3 const &a = world_from_local * glm::vec4(t.a, 1.0f);
            glm::vec3 const &b = world_from_local * glm::vec4(t.b, 1.0f);
            glm::vec3 const &c = world_from_local * glm::vec4(t.c, 1.0f);

            //get barycentric coordinates of closest point in the plane of (a,b,c):
            glm::vec3 coords = barycentric_weights(a,b,c, center);

            glm::vec3 coords_to_world = (coords.x * a
                + coords.y * b
                + coords.z * c);

            //is that point inside the triangle?
            if (coords.x >= 0.0f && coords.y >= 0.0f && coords.z >= 0.0f) {
                //yes, point is inside triangle.
                float dis2 = glm::length2(center - coords_to_world);
                if(dis2 < glm::length2(radius)){
                    //collided
                    if (dis2 < closest_dis2) {
                        closest_dis2 = dis2;
                        closest = coords_to_world;
                        closest_point_normal = get_world_normal(t, coords, world_from_local);
                    }
                }
            } else {
                //check triangle vertices and edges:
                auto check_edge = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c) {

                    //find closest point on line segment ab:
                    float along = glm::dot(center-a, b-a);
                    float max = glm::dot(b-a, b-a);
                    glm::vec3 pt;
                    glm::vec3 coords;
                    if (along < 0.0f) {
                        pt = a;
                        coords = glm::vec3(1.0f, 0.0f, 0.0f);
                    } else if (along > max) {
                        pt = b;
                        coords = glm::vec3(0.0f, 1.0f, 0.0f);
                    } else {
                        float amt = along / max;
                        pt = glm::mix(a, b, amt);
                        coords = glm::vec3(1.0f - amt, amt, 0.0f);
                    }

                    float dis2 = glm::length2(center - pt);
                    if(dis2 < glm::length2(radius)){
                        //collided
                        if (dis2 < closest_dis2) {
                            closest_dis2 = dis2;
                            closest = pt;
                            closest_point_normal = get_world_normal(t, coords, world_from_local);
                        }
                    }
                    
                };
                check_edge(a, b, c);
                check_edge(b, c, a);
                check_edge(c, a, b);
            }
        }
    }


	return {closest, closest_point_normal};

    /////////

    // //loop over triangles in mesh
    // for(Triangle t : other_mesh->triangles){

    //     glm::vec3 triangle_a = transform_other->make_local_to_world() * glm::vec4(t.a, 1.0f);
    //     glm::vec3 triangle_b = transform_other->make_local_to_world() * glm::vec4(t.b, 1.0f);
    //     glm::vec3 triangle_c = transform_other->make_local_to_world() * glm::vec4(t.c, 1.0f);

    //     glm::vec3 coords = barycentric_weights(triangle_a,triangle_b,triangle_c,center);
    //     float radius = puffer->current_scale*4.0f;
        
    //     //is that point inside the triangle?
	// 	if (coords.x >= 0.0f && coords.y >= 0.0f && coords.z >= 0.0f) {
	// 		//yes, point is inside triangle.
    //         glm::vec3 coords_to_world = (coords.x * triangle_a
	// 	     + coords.y * triangle_b
	// 	     + coords.z * triangle_c);

	// 		float dis = glm::length(center - coords_to_world);
	// 		if (dis < radius) {
	// 			return coords_to_world;
	// 		}
    //     }
    // }

    // return glm::vec3(0,0,0);
}

bool CollisionDetector::check_ray_mesh_collision(const glm::vec3 p0, const glm::vec3 dir, const Scene::Transform *transform_other, const Mesh *other_mesh, float& t)
{
    assert(other_mesh);

    t = std::numeric_limits<float>::infinity();
    bool collided = false;
	for (Triangle triangle : other_mesh->triangles) {
		//find closest point on triangle:
        glm::mat4x4 world_from_local = transform_other->make_local_to_world();
		glm::vec3 const &a = world_from_local * glm::vec4(triangle.a, 1.0f);
		glm::vec3 const &b = world_from_local * glm::vec4(triangle.b, 1.0f);
		glm::vec3 const &c = world_from_local * glm::vec4(triangle.c, 1.0f);
        float cur_t;
        if (line_triangle_intersection(p0, dir, a, b, c, cur_t) && cur_t < t) {
            t = cur_t;
            collided = true;
        }
	}
    return collided;
}

bool CollisionDetector::check_over_water(const Scene::Transform *water_transform, const Mesh *water_mesh)
{
    float radius = puffer->current_scale*4.0f;
    glm::vec3 water_level = glm::vec3(water_transform->make_local_to_world() * glm::vec4(water_mesh->max, 1.0f));
    return water_level.z < (puffer->get_position().z - (radius/2.0f));
}

//referenced https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool CollisionDetector::line_triangle_intersection(
    const glm::vec3& P0, const glm::vec3& dir,   // Line point and direction
    const glm::vec3& V0, const glm::vec3& V1, const glm::vec3& V2, // Triangle vertices
    float& time)                     // Output: intersection point
{
    const float EPSILON = 1e-5f;
    
    glm::vec3 edge1 = V1 - V0;
    glm::vec3 edge2 = V2 - V0;

    glm::vec3 h = glm::cross(dir, edge2);
    float a = glm::dot(edge1, h);

    // parallel to triangle
    if (std::abs(a) < EPSILON)
        return false;

    float f = 1.0f / a;
    glm::vec3 s = P0 - V0;
    float u = f * glm::dot(s, h);

    // intersection is outside the triangle
    if (u < 0.0f || u > 1.0f)
        return false;

    glm::vec3 q = glm::cross(s, edge1);
    float v = f * glm::dot(dir, q);

    // intersection is outside the triangle
    if (v < 0.0f || u + v > 1.0f)
        return false;

    float t = f * glm::dot(edge2, q);

    if (t > EPSILON) {
        time = t;
        return true;
    }

    return false;
}