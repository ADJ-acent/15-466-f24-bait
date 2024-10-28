#include "QTE.hpp"

void QTE::start() {
    active = true;
    success = false;
    timer = time_limit;
    input_delay = 2.0f;

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
        success = true;
        std::cout << "QTE Success!" << std::endl;
        end();
        return;
    }
    
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (state[i] && SDL_GetKeyFromScancode(static_cast<SDL_Scancode>(i)) != required_key) {
            // A key is pressed and it's not the required key
            std::cout << "QTE Failed! Wrong Key Pressed!" << std::endl;
            end();
            return;  // End the QTE if failure
        }
    }

    if (timer <= 0.0f && !success) {
        std::cout << "QTE Failed! Time's up!" << std::endl;
        end();
        return;
    }
}

void QTE::end(){
        active = false;
}