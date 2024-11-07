#include "QTE.hpp"

int QTE::score = 0;

void QTE::start() {
    active = true;
    success = false;
    failure = false;
    timer = time_limit;
    red_text_percentage = 0.0f;
    input_delay = 2.0f;
    bait->is_active = false;
    
    std::srand(uint32_t(std::time(0)));
    int random_index = std::rand() % possible_keys.size();
    required_key = possible_keys[random_index];
}

void QTE::update(float elapsed) {
    if (!active) return;

    if(failure){
        bait_hook_up(elapsed);
        return;
    }
    else if(success){
        bait_eaten();
        return;
    }

    if(input_delay > 0){
        input_delay -= elapsed;
        return;
    }  

    //std::cout << get_prompt() << std::endl;
    timer -= elapsed;
    red_text_percentage += elapsed;

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    
    if (state[SDL_GetScancodeFromKey(required_key)]) {

        QTE::score++; 
        bait->bait_bites_left--;
        bait->mesh_parts.bait_base->scale *= 0.8; // scale down the bait whenever a QTE succeeds
        std::cout << "scaled down" << std::endl;
        std::cout << bait->bait_bites_left << std::endl;

        if(bait->bait_bites_left == 0){
            success = true;
            return;
        }

        reset();
        return;
    }
    
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (state[i] && SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) != required_key) {
            std::cout << "QTE Failed! Wrong Key Pressed!" << std::endl;
            failure = true;
            return;
        }
    }

    if (timer <= 0.0f && !success) {
        std::cout << "QTE Failed! Time's up!" << std::endl;
        failure = true;
        return;
    }
}

void QTE::reset(){
    input_delay = 2.0f;
    timer = time_limit;
    red_text_percentage = 0.0f;
    std::srand(uint32_t(std::time(0)));
    int random_index = std::rand() % possible_keys.size();
    required_key = possible_keys[random_index];
}

void QTE::bait_hook_up(float elapsed){
    //glm::vec3 move_up_speed = glm::vec3(0.0f, 0.0f, 10.0f);
    if(hook_up_timer < 5.0f) {
        // puffer->main_transform->position = bait->mesh_parts.bait_base->make_local_to_world() * glm::vec4(bait->mesh_parts.bait_base->position,1.0f);
        
        // bait->mesh_parts.bait_string->position += move_up_speed * elapsed;

        bait->reel_up(elapsed);
        hook_up_timer += elapsed;
    }
    else{
        end();
    }
}

void QTE::bait_eaten(){
    bait->mesh_parts.bait_base->scale *= 0;
    end();
}

void QTE::end(){
    active = false;
}