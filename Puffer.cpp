#include "Puffer.hpp"
#include "GameConfig.hpp"
#include "math_helpers.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp> 
// #include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <iostream>

extern Load< MeshBuffer > pufferfish_meshes;
extern GameConfig game_config;

void Puffer::init(std::vector< Scene::Transform * > transform_vector, Scene *scene_)
{
    assign_mesh_parts(transform_vector);
    scene = scene_;
    puffer_collider.init(this, main_transform,pufferfish_meshes->lookup("PuffBody")); 

    original_mesh_scale = mesh->scale;
    original_mesh_position = mesh->position;
    original_mesh_rotation = mesh->rotation * glm::angleAxis(glm::radians(180.0f), glm::vec3(0.0f,1.0f,0.0f));
    mesh->rotation = original_mesh_rotation;
    base_rotation = original_mesh_rotation;
    original_rotation = main_transform->rotation;
    default_spring_arm_length = glm::length(camera->position);
    spring_arm_normalized_displacement = camera->position / default_spring_arm_length;
    
    
    { //set up build up animations
        build_up_animations.reserve(10);
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(1.0f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(2.0f)},
            }, 
            &mesh_parts.puff_body->scale)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(-3.1608f, -0.18379f, 2.8906f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(-5.3108f, -0.18379f, 7.19066f)},
            }, 
            &mesh_parts.puff_l_blush->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(-3.1889f, 0.16394f, 2.919f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(-5.3389f, 0.16394f, 7.204f)},
            }, 
            &mesh_parts.puff_l_eye->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(-3.9862f,-0.005412f, -0.30636f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(-7.4861f, -0.005412f, -3.3064f)},
            }, 
            &mesh_parts.puff_l_fin->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(0.0f,0.0f, 4.8114f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(0.0f,0.0f, 9.4984f)},
            }, 
            &mesh_parts.puff_mouth->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(1.0f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(0.6f)},
            }, 
            &mesh_parts.puff_mouth->scale)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(3.1608f, -0.18379f, 2.8906f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(5.3108f, -0.18379f, 7.19066f)},
            }, 
            &mesh_parts.puff_r_blush->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(3.1889f, 0.16394f, 2.919f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(5.3389f, 0.16394f, 7.204f)},
            }, 
            &mesh_parts.puff_r_eye->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(3.9862f,-0.005412f, -0.30636f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(7.4861f, -0.005412f, -3.3064f)},
            }, 
            &mesh_parts.puff_r_fin->position)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(1.0f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(2.0f)},
            }, 
            &mesh_parts.puff_spikes->scale)
        );
        build_up_animations.push_back(LinearAnimation({
                LinearFrame<glm::vec3>{0.0f, glm::vec3(-0.02516f, -0.045457f, -4.4023f)},
                LinearFrame<glm::vec3>{1.0f, glm::vec3(-0.02516f, -0.045457f, -8.9023f)},
            }, 
            &mesh_parts.puff_tail->position)
        );
    }

    {// set up swim animation
        swim_animation.push_back(SlerpAnimation({
                SlerpFrame{0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
                SlerpFrame{0.1f, glm::quat(0.86f, 0.0f, -0.51f, 0.0f)},
                SlerpFrame{0.3f, glm::quat(0.981f, 0.0f, 0.194f, 0.0f)},
                SlerpFrame{0.8f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
            }, 
            &mesh_parts.puff_l_fin->rotation)
        );
        swim_animation.push_back(SlerpAnimation({
                SlerpFrame{0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
                SlerpFrame{0.1f, glm::quat(0.86f, 0.0f, 0.51f, 0.0f)},
                SlerpFrame{0.3f, glm::quat(0.981f, 0.0f, -0.194f, 0.0f)},
                SlerpFrame{0.8f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
            }, 
            &mesh_parts.puff_r_fin->rotation)
        );
        swim_animation.push_back(SlerpAnimation({
                SlerpFrame{0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
                SlerpFrame{0.1f, glm::quat(0.86f, 0.0f, 0.51f, 0.0f)},
                SlerpFrame{0.3f, glm::quat(0.981f, 0.0f, -0.194f, 0.0f)},
                SlerpFrame{0.8f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
                SlerpFrame{0.9f, glm::quat(0.86f, 0.0f, -0.51f, 0.0f)},
                SlerpFrame{0.11f, glm::quat(0.981f, 0.0f, 0.194f, 0.0f)},
                SlerpFrame{0.16f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
            }, 
            &mesh_parts.puff_tail->rotation)
        );
        swim_animation.push_back(SlerpAnimation({
                SlerpFrame{0.0f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
                SlerpFrame{0.1f, glm::quat(0.86f, 0.0f, -0.51f, 0.0f)},
                SlerpFrame{0.3f, glm::quat(0.981f, 0.0f, 0.194f, 0.0f)},
                SlerpFrame{0.8f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
                SlerpFrame{0.9f, glm::quat(0.86f, 0.0f, 0.51f, 0.0f)},
                SlerpFrame{0.11f, glm::quat(0.981f, 0.0f, -0.194f, 0.0f)},
                SlerpFrame{0.16f, glm::quat(1.0f, 0.0f, 0.0f, 0.0f)},
            }, 
            &mesh_parts.puff_tail->rotation)
        );
    }
}

void Puffer::rotate_from_mouse(glm::vec2 mouse_motion)
{
    assert(main_transform);
    if (mouse_motion == glm::vec2(0.0f)) return;
    constexpr float sensitivity = 60.0f;

    float yaw_delta = -mouse_motion.x * sensitivity;
    float pitch_delta = mouse_motion.y * sensitivity;

    // clamp the pitch and recalculate delta
    current_yaw = fmod(current_yaw + yaw_delta, 360.0f);
    current_pitch = glm::clamp(current_pitch + pitch_delta, -89.9f, 89.9f);

    glm::quat yaw_rotation = glm::angleAxis(glm::radians(current_yaw), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::quat pitch_rotation = glm::angleAxis(glm::radians(current_pitch), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::quat total_rotation = yaw_rotation * pitch_rotation;
    glm::quat old_rotation = main_transform->rotation;
    main_transform->rotation =  glm::normalize(total_rotation * original_rotation);
    glm::quat new_to_old = glm::inverse(main_transform->rotation) * old_rotation;

    // keep mesh stationary in world position
    base_rotation*= new_to_old;
    if (!building_up && swim_cooldown == 0.0f) { // when building up, orient the fish tail to the screen
        mesh->rotation = base_rotation;
    }
}

void Puffer::start_build_up()
{
    assert(main_transform);
    if (build_up_time == 0.0f && recovered) {
        building_up = true;
        recovered = false;
    }
}

void Puffer::release()
{
    assert(main_transform);
    if (building_up) {
        building_up = false;
        overshoot = true;
        overshoot_target = 1.0f - (current_scale - 1.0f)/2.0f;
        velocity = get_forward() * speed * build_up_time;
        mesh->position = original_mesh_position;
        release_rotate_angle = 20.0f * build_up_time;
        total_release_angle = 0.0f;
        release_rotate_axis = glm::normalize(glm::linearRand(glm::vec3(-1.0f), glm::vec3(1.0f)));
        base_rotation = mesh->rotation;
    }
}

void Puffer::update(glm::vec2 mouse_motion, int8_t swim_direction, float elapsed)
{
    assert(main_transform);
    idletime += elapsed;

    rotate_from_mouse(mouse_motion);

    constexpr float swim_cooldown_threshold = 0.5f;

    {// puffer collision
        bool colliding = false;
        float best_spring_arm_length = default_spring_arm_length;
        for (Scene::Drawable &d : scene->drawables){
            assert(d.mesh);
            if (!colliding) {
                bool checking_mesh_in_puffer = false;
                for(std::string name : names){
                    if(name == d.transform->name){
                        checking_mesh_in_puffer = true;
                    }
                }
                //check that its not seaweed
                bool checking_non_colliding_object = false;
                if((d.transform->name.substr(0,7) == "seaweed") || (d.transform->name.substr(0,5)=="water")){
                    checking_non_colliding_object = true;
                }

                float bounce_factor = 1.0f;
                if(d.transform->name.substr(0,4)=="sand"){
                    bounce_factor = 0.1f;
                }

                if(d.transform->name.substr(0,5)=="water"){
                    checking_non_colliding_object = true;
                    above_water = puffer_collider.check_over_water(d.transform,d.mesh);
                }

                if(!checking_mesh_in_puffer && !checking_non_colliding_object){
                    std::array<glm::vec3, 2> collision_point = puffer_collider.check_collision(d.transform,d.mesh);
                    if(collision_point[0] != glm::vec3(std::numeric_limits<float>::infinity())){
                        colliding = true;
                        //std::cout << "collision point x: " << collision_point[0].x << std::endl;
                       // std::cout << "collision point y: " << collision_point[0].y << std::endl;
                       // std::cout << "collision point z: " << collision_point[0].z << std::endl;
                       // std::cout << std::endl;
                    }
                    if (colliding){
                        handle_collision(collision_point,bounce_factor);
                    }
                    glm::vec3 p0 = get_position();
                    glm::vec3 camera_pos = camera->make_local_to_world() * glm::vec4(camera->position,1.0f);
                    glm::vec3 dir = glm::normalize(p0 - camera_pos);
                    float t;
                    if (puffer_collider.check_ray_mesh_collision(p0,dir,d.transform,d.mesh, t)) {
                        if (t < best_spring_arm_length) {
                            best_spring_arm_length = t;
                        }
                    }
                }
            }	
        }
        camera->position = spring_arm_normalized_displacement * std::max(0.01f, best_spring_arm_length + 0.1f);
    }

    if (swim_cooldown == 0.0f) {
        if (swim_direction != 0) {
            swim(swim_direction);
            swim_cooldown = 0.001f; // increment slightly to start the timer
        }
    }
    else {
        swim_cooldown += elapsed;
        swim_animation[swimming_side].update(swim_cooldown);
        swim_animation[swimming_side + 2].update(swim_cooldown);
        if (swim_cooldown > swim_cooldown_threshold) {
            swim_cooldown = 0.0f;
        }
        else if (swim_cooldown < 0.2f) {
            mesh->rotation = base_rotation * glm::angleAxis((float(swimming_side) - 0.5f) * 20.0f * swim_cooldown * swim_cooldown , glm::vec3(0.0f,1.0f, 0.0f));
        }
    }

    {// handle movement
        float velocity_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_velocity_halflife * 2.0f));
        if(above_water){
            velocity -= glm::vec3(0.0f,0.0f,gravity)*elapsed;
        } else {
            velocity = glm::mix(velocity, glm::vec3(0.0f), velocity_amt);
        }
        
        
        main_transform->position += velocity;
        
        if (building_up) {
            
            build_up_time += elapsed * 0.5f;
            // grow
            if (build_up_time <= 1.0f) {
                // std::cout<<(1.0f+cubic_bezier(0.0f,0.5f, 0.34f, 1.0f, build_up_time))<<std::endl;
                current_scale = 1.0f + cubic_bezier(0.0f,0.8f, 0.9f, 1.0f, build_up_time);
                mesh->position = original_mesh_position + calculate_jitter(elapsed);
            }
            // stay at peak for 1.5 seconds
            else if (build_up_time <= 1.5f) {
                // just shake
                mesh->position = original_mesh_position + calculate_jitter(elapsed);
            }
            // shrink
            else {
                mesh->position = original_mesh_position;
                release();
            }
        }
        // shrink phase
        else if (!recovered) {
            //recover from overshoot
            if (current_scale < 1.0f && !overshoot) {
                float amt = 1.0f - std::pow(0.5f, elapsed / (puffer_scale_recover_halflife * 2.0f));
                current_scale = glm::mix(current_scale, 1.1f, amt);
                //fully recovered
                if (current_scale > 0.99f) {
                    recovered = true;
                    current_scale = 1.0f;
                    build_up_time = 0.0f;
                }
            }
            else {
                float amt = 1.0f - std::pow(0.5f, elapsed / (puffer_scale_decay_halflife * 2.0f));
               
                current_scale = glm::mix(current_scale, overshoot_target, amt);
                if (current_scale < overshoot_target + 0.01f) {
                    overshoot = false;
                }
            }
        }
    }

    {// update animation based on scale of the pufferfish
        update_build_up_animations(current_scale-1.0f);
        if (current_scale < 1.0f) {
            mesh->scale = original_mesh_scale * current_scale;
        }
    }

    {// mesh rotation
        float rotation_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_rotation_release_halflife * 2.0f));
        //THIS ONE IF FOR ROTATE TOWARDS CAMERA
        if (building_up) { // experimental...conflicted on how this feels
            if (game_config.charge_face_camera) {
                mesh->rotation = glm::slerp(mesh->rotation, glm::angleAxis(glm::radians(180.0f),glm::vec3(0.0f,1.0f,0.0f)) * original_mesh_rotation, rotation_amt);
            }
            else {
                mesh->rotation = glm::slerp(mesh->rotation, original_mesh_rotation, rotation_amt);
            }
        }

        else if (release_rotate_angle > 1.0f) {
                rotation_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_rotation_release_halflife * 2.0f));
                total_release_angle += elapsed * release_rotate_angle;
                mesh->rotation = base_rotation * glm::angleAxis(total_release_angle, release_rotate_axis);
                release_rotate_angle = glm::mix(release_rotate_angle, 0.0f, rotation_amt);
                if (release_rotate_angle <= 1.0f) {
                    base_rotation = mesh->rotation;
                }
            
        }
        else {//only return to tail view when we aren't rolling
            // update mesh rotation to return to normal (if we rotated camera recently)
            mesh->position.y = original_mesh_position.y + sin(idletime * 2.0f);
            rotation_amt = 1.0f - std::pow(0.5f, elapsed / (puffer_rotation_return_halflife * 2.0f));
            mesh->rotation = glm::slerp(mesh->rotation, glm::angleAxis(glm::radians(15.0f) * sin(idletime * 2.0f) ,glm::vec3(1.0f,0.0f,0.0f)) *  original_mesh_rotation, rotation_amt);
            total_release_angle = 0.0f;
            if (swim_cooldown == 0.0f) {
                base_rotation = mesh->rotation;
            }
        }
    }

}

void Puffer::handle_collision(std::array<glm::vec3,2> collision_point,float bounce_factor)
{
    glm::vec3 collision_direction = glm::normalize(get_position() - collision_point[0]);
    glm::vec3 n = glm::normalize(collision_point[1]);
    float radius = current_scale*4.3f;
    if(glm::length(velocity) <= 0.01f){
        //if puffing up
        velocity = n * .01f * speed * bounce_factor;
    } else {
        glm::vec3 v_normal = glm::dot(velocity, n) * n;
        glm::vec3 v_tangent = (velocity - v_normal);
        glm::vec3 v_normal_reflected = -bounce_factor * v_normal;
        glm::vec3 v_final = v_normal_reflected + v_tangent;
        velocity = v_final;
    }
    main_transform->position = collision_point[0] + collision_direction * (radius+ 0.2f);

}

void Puffer::update_build_up_animations(float t)
{
    for (LinearAnimation<glm::vec3>& animation : build_up_animations) {
        animation.update(t);
    }
}

//swim direction -1 for left, 1 for right
void Puffer::swim(int8_t swim_direction)
{
    float build_up_penaulty = 1.0f / current_scale;
    swimming_side = (-swim_direction + 1) / 2;
    velocity += get_forward() * (0.15f * build_up_penaulty) + (float(swim_direction) * 0.05f * build_up_penaulty) * get_right();
    if (building_up) {
        base_rotation = mesh->rotation;
    }
}

void Puffer::switch_to_main_menu_camera()
{
    camera->position = spring_arm_normalized_displacement * 5.0f;
}

void Puffer::switch_to_default_camera()
{
    camera->position = spring_arm_normalized_displacement * default_spring_arm_length;
}

void Puffer::assign_mesh_parts(std::vector< Scene::Transform * > transform_vector)
{
    for (auto t : transform_vector){
        // transform_vector.back()
        if (t->name == "PuffMain") {
            main_transform = t;
        }
        else if (t->name == "PuffMesh") {
            mesh = t;
        }
        else if (t->name == "PuffCam") {
            camera = t;
        }
        else if (t->name == "PuffBody") {
            mesh_parts.puff_body = t;
        }
        else if (t->name == "PuffLBlush") {
            mesh_parts.puff_l_blush = t;
        }
        else if (t->name == "PuffLEye") {
            mesh_parts.puff_l_eye = t;
        }
        else if (t->name == "PuffLFin") {
            mesh_parts.puff_l_fin = t;
        }
        else if (t->name == "PuffMouth") {
            mesh_parts.puff_mouth = t;
        }
        else if (t->name == "PuffRBlush") {
            mesh_parts.puff_r_blush = t;
        }
        else if (t->name == "PuffREye") {
            mesh_parts.puff_r_eye = t;
        }
        else if (t->name == "PuffRFin") {
            mesh_parts.puff_r_fin = t;
        }
        else if (t->name == "PuffSpikes") {
            mesh_parts.puff_spikes = t;
        }
        else if (t->name == "PuffTail") {
            mesh_parts.puff_tail = t;
        }

    }
    
}

glm::vec3 Puffer::calculate_jitter(float elapsed)
{
    static float time = 0.0f;
    time += elapsed;
    time = fmodf(time, 10.0f);
    constexpr float total_amplitude = .5f;
    float shake_intensity;
    if (build_up_time < 1.0f) {
        shake_intensity = build_up_time * total_amplitude;
    }
    else {
        shake_intensity = total_amplitude;
    }
    // Generate Perlin noise for each axis, adding multiple frequencies
    glm::vec3 shake = glm::vec3(0.0f);
    float frequency = 10.0f;
    float amplitude = 20.0f;

    float shake_1 = glm::perlin(glm::vec3(time) * frequency * amplitude);
    shake += glm::vec3(-shake_1, -shake_1, shake_1);

    amplitude *= 0.5f;
    frequency *= 2.0f;
    float shake_2 = glm::perlin(glm::vec3(time+10.0f) * frequency * amplitude);
    shake += glm::vec3(shake_2, -shake_2, -shake_2);

    amplitude *= 0.5f;
    frequency *= 2.0f;
    float shake_3 = glm::perlin(glm::vec3(time+20.0f) * frequency * amplitude);
    shake += glm::vec3(-shake_3, shake_3, -shake_3);

    // Normalize the shake so it doesn't exceed the expected intensity
    shake *= shake_intensity;  // Apply the shake intensity
    return shake;
}

glm::vec3 Puffer::get_forward()
{
    return main_transform->rotation * glm::vec3(0,0,1);
}

glm::vec3 Puffer::get_right()
{
    return main_transform->rotation * glm::vec3(-1,0,0);
}

glm::vec3 Puffer::get_position()
{
    return main_transform->position;
}