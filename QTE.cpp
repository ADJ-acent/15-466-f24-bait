#include "QTE.hpp"

void QTE::start(int goal) {
    active = true;
    std::cout << "QTE is active now" << std::endl;
    success = false;
    failure = false;
    timer = time_limit;
    input_delay = 2.0f;
    success_count = 0;
    success_count_goal = goal;

    std::srand(std::time(0));
    int random_index = std::rand() % possible_keys.size();
    required_key = possible_keys[random_index];
}

void QTE::update(float elapsed) {
    if (!active) return;

    if(failure){
        bait_hook_up(elapsed);
    }
    else if(success){
        bait_eatten();
    }

    if(input_delay > 0){
        input_delay -= elapsed;
        return;
    }  

    std::cout << get_prompt() << std::endl;
    timer -= elapsed;

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    
    if (state[SDL_GetScancodeFromKey(required_key)]) {

        success_count++;
        bait->scale *= 0.8; // scale down the bait whenever a QTE succeeds

        if(success_count == success_count_goal){
            success = true;
        }

        reset();
        std::cout << success_count << std::endl;
        
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
    std::srand(std::time(0));
    int random_index = std::rand() % possible_keys.size();
    required_key = possible_keys[random_index];
}

void QTE::bait_hook_up(float elapsed){
    glm::vec3 move_up_speed = glm::vec3(0.0f, 0.0f, 3.0f);
    if(hook_up_timer < 3.0f) {
        // fish->position += move_up_speed * elapsed;
        rope->position += move_up_speed * elapsed;
        hook_up_timer += elapsed;
    }
    else{
        end();
    }
}

void QTE::bait_eatten(){
    bait->scale *= 0;
    score++;
    end();
}


void QTE::end(){
    active = false;
}