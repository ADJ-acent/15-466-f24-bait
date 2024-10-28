#include "QTE.hpp"

void QTE::start() {
    active = true;
    success = false;
    timer = time_limit;
    input_delay = 2.0f;
    success_count = 0;
    success_count_goal = 3;

    std::srand(std::time(0));
    int random_index = std::rand() % possible_keys.size();
    required_key = possible_keys[random_index];
}

void QTE::update(float elapsed) {
    if (!active) return;

    if(input_delay > 0){
        input_delay -= elapsed;
        return;
    }  

    std::cout << get_prompt() << std::endl;
    timer -= elapsed;

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    
    if (state[SDL_GetScancodeFromKey(required_key)]) {
        success_count++;
        if(success_count == success_count_goal){
            success = true;
            end();
        }

        reset();
        std::cout << success_count << std::endl;
        
        return;
    }
    
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (state[i] && SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) != required_key) {
            std::cout << "QTE Failed! Wrong Key Pressed!" << std::endl;
            end();
            return;
        }
    }

    if (timer <= 0.0f && !success) {
        std::cout << "QTE Failed! Time's up!" << std::endl;
        end();
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

void QTE::end(){
    active = false;
}