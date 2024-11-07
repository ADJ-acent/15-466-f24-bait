#include "QTE.hpp"

int QTE::score = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist_index(0, 3);

void QTE::start() {
    active = true;
    success = false;
    failure = false;
    correct_key_pressed = false;

    timer = 0.0f;
    red_percentage = 0.0f;
    input_delay = 2.0f;

    bait->is_active = false;

    int random_index = dist_index(gen);
    required_key = possible_keys[random_index];
}

void QTE::update(float elapsed) {
    if (!active) return;

    timer += elapsed;
    red_percentage = timer/time_limit;

    if(failure){
        if(timer >= time_limit) {
            bait_hook_up(elapsed);
            return;
        }
        else{
            return;
        }
    }
    else if(success){
        if(timer >= time_limit) {
            end();
            return;
        }
        else{
            return;
        }
    }

    if(input_delay > 0){
        input_delay -= elapsed;
        reset();
        return;
    }

    if(!correct_key_pressed) {
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        
        if (state[SDL_GetScancodeFromKey(required_key)]) {

            correct_key_pressed = true;
            QTE::score++; 
            bait->bait_bites_left--;
            if(bait->bait_bites_left > 0)   {
                bait->mesh_parts.bait_base->scale *= 0.8;   // scale down the bait whenever a QTE succeeds
            } 
            else{
                bait->mesh_parts.bait_base->scale *= 0;
            }
            
            std::cout << "scaled down" << std::endl;
            std::cout << bait->bait_bites_left << std::endl;

            if(bait->bait_bites_left == 0){
                success = true;
            }

            return;
        }
        
        for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
            if (state[i] && SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) != required_key) {
                std::cout << "QTE Failed! Wrong Key Pressed!" << std::endl;
                failure = true;
                return;
            }
        }

        if (timer >= time_limit && !correct_key_pressed) {
            std::cout << "QTE Failed! Time's up!" << std::endl;
            failure = true;
            return;
        }
    }
    else{
        if(timer >= time_limit && !success){
            std::cout << "reset" << std::endl;
            input_delay = 2.0f;
            reset();
            return;
        }
    }
}

void QTE::reset(){
    // input_delay = 2.0f;
    correct_key_pressed = false;
    timer = 0.0f;
    red_percentage = 0.0f;

    int random_index = dist_index(gen);
    required_key = possible_keys[random_index];
}

void QTE::bait_hook_up(float elapsed){
    //glm::vec3 move_up_speed = glm::vec3(0.0f, 0.0f, 10.0f);
    if(reel_up_timer < 5.0f) {
        // puffer->main_transform->position = bait->mesh_parts.bait_base->make_local_to_world() * glm::vec4(bait->mesh_parts.bait_base->position,1.0f);
        
        // bait->mesh_parts.bait_string->position += move_up_speed * elapsed;

        bait->reel_up(elapsed);
        reel_up_timer += elapsed;
    }
    else{
        end();
    }
}

void QTE::end(){
    if(bait->bait_bites_left > 0){
        bait->is_active = true;
    }
    else{
        bait->is_active = false;
    }

    active = false;
}