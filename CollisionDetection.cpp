#include "CollisionDetection.hpp"
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

void CollisionDetector::calculate_AABB(Scene::Transform *transform_, const Mesh &mesh)
{
    glm::vec3 local_min = mesh.min;
    glm::vec3 local_max = mesh.max;

    glm::vec3 world_min = glm::vec3(transform->make_local_to_world() * glm::vec4(local_min, 1.0f));
    glm::vec3 world_max = glm::vec3(transform->make_local_to_world() * glm::vec4(local_max, 1.0f));

    aabb.max = world_max;
    aabb.min = world_min;
}

void CollisionDetector::init(Puffer &p, Scene::Transform *t, const Mesh &m)
{
    //get radius
    transform = t; //store pointer to transform
    puffer = &p;

    glm::vec3 local_min = m.min;
    glm::vec3 local_max = m.max;
    glm::vec3 world_min = glm::vec3(transform->make_local_to_world() * glm::vec4(local_min, 1.0f));
    glm::vec3 world_max = glm::vec3(transform->make_local_to_world() * glm::vec4(local_max, 1.0f));
    
    aabb.max = world_max;
    aabb.min = world_min;
   

}

glm::vec3 CollisionDetector::check_collision(const Scene::Transform *transform_other, const Mesh *other_mesh)
{
    // find barycentric coordinates of center of sphere on the triangle 
    // then checking distance
    // if lesser than radius then colliding

    //recalculate center and positions given current position
    // calculate_AABB(transform,mesh);
    glm::vec3 center = transform->position;
    assert(other_mesh);
    assert(other_mesh);

    // for(GLuint i = other_mesh->start; i < other_mesh->count+other_mesh->start; i+=3){
        
    //     glm::vec3 triangle_a = transform_other->make_local_to_world() * glm::vec4(meshbuffer->data[i].Position, 1.0f);
    //     glm::vec3 triangle_b = transform_other->make_local_to_world() * glm::vec4(meshbuffer->data[i+1].Position, 1.0f);
    //     glm::vec3 triangle_c = transform_other->make_local_to_world() * glm::vec4(meshbuffer->data[i+2].Position, 1.0f);

    //     glm::vec3 coords = barycentric_weights(triangle_a,triangle_b,triangle_c,center);
    //     float radius = puffer->current_scale*4.f;

    //     //is that point inside the triangle?
	// 	if (coords.x >= 0.0f && coords.y >= 0.0f && coords.z >= 0.0f) {
	// 		//yes, point is inside triangle.
    //         glm::vec3 coords_to_world = (coords.x * triangle_a
	// 	     + coords.y * triangle_b
	// 	     + coords.z * triangle_c);

	// 		float dis = glm::length(center - coords_to_world);
    //         std::cout<<"dis: " << dis <<std::endl;  
    //         std::cout<<"radius: " << radius <<std::endl;  
	// 		if (dis < radius) {
    //             std::cout<<"Colliding!!"<<std::endl;  
	// 			return true;
	// 		}
    //     }

    // }

    //loop over triangles in mesh
    for(Triangle t : other_mesh->triangles){

        glm::vec3 triangle_a = transform_other->make_local_to_world() * glm::vec4(t.a, 1.0f);
        glm::vec3 triangle_b = transform_other->make_local_to_world() * glm::vec4(t.b, 1.0f);
        glm::vec3 triangle_c = transform_other->make_local_to_world() * glm::vec4(t.c, 1.0f);

        glm::vec3 coords = barycentric_weights(triangle_a,triangle_b,triangle_c,center);
        float radius = puffer->current_scale*4.f;
        
        //is that point inside the triangle?
		if (coords.x >= 0.0f && coords.y >= 0.0f && coords.z >= 0.0f) {
			//yes, point is inside triangle.
            glm::vec3 coords_to_world = (coords.x * triangle_a
		     + coords.y * triangle_b
		     + coords.z * triangle_c);

			float dis = glm::length(center - coords_to_world);
			if (dis < radius) {
				return coords_to_world;
			}
        }
    }

    return glm::vec3(0,0,0);
}
