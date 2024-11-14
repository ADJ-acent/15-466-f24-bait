#include "QTE.hpp"

int QTE::score = 100;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist_index(0, 3);

void QTE::start() {
    active = true;
    success = false;
    failure = false;
    correct_key_pressed = false;
    trap_key_on = false;

    timer = 0.0f;
    red_percentage = 0.0f;
    input_delay = input_delay_time;

    bait->is_active = false;
    bait->currently_in_qte = true;

    int random_index = dist_index(gen);
    required_key = possible_keys[random_index];
}

void QTE::update(float elapsed) {
    if (!active) return;

    timer += elapsed;
    red_percentage = timer/time_limit;

    if(failure){
        bait_hook_up(elapsed);
        return;
    }

    if(success){
        if(timer >= time_limit) {
            end();
            return;
        }
        else{
            return;
        }
    }

    if(input_delay > 0.0f){
        input_delay -= elapsed;
        key_flash_reset_timer -= elapsed;

        if(key_flash_reset_timer <= 0.0f) {
            flash_times += 1;

            key_flashing_reset();
            if(input_delay > 1.5f) {
                key_flash_reset_timer = cubic_bezier(0.7f, 0.03f, 0.92f, 0.13f, 0.5f) / 10.0f;
            }
            else if(input_delay > 0.8f){
                key_flash_reset_timer = cubic_bezier(0.7f, 0.03f, 0.92f, 0.13f, 0.5f) / 5.0f;
            }
            else{
                key_flash_reset_timer = cubic_bezier(0.7f, 0.03f, 0.92f, 0.13f, 0.0f) / 2.5f;
            }

            std::cout << key_flash_reset_timer << std::endl;

        }
        return;
    }
    else if(input_delay <= 0.0f && !key_reset){
        reset();
        key_reset = true;
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
            input_delay = input_delay_time;
            key_flash_reset_timer = cubic_bezier(0.7f, 0.03f, 0.92f, 0.13f, 1.0f) / 10.0f;
            reset();
            key_reset = false;
            return;
        }
    }
}

void QTE::reset(){
    correct_key_pressed = false;
    timer = 0.0f;
    red_percentage = 0.0f;
    flash_times = 0;

    int random_index = dist_index(gen);
    required_key = possible_keys[random_index];
    int random_trap_index = dist_index(gen);
    trap_key = possible_keys[random_index];

    if(random_trap_index == random_index){
        trap_key_on = true;
    }
}

void QTE::key_flashing_reset(){
    timer = 0.0f;

    required_key = possible_keys[flashing_key_index];
    flashing_key_index += 1;
    if(flashing_key_index == 4){
        flashing_key_index = 0;
    }
}

void QTE::bait_hook_up(float elapsed){
    bait->reel_up(elapsed);
    if(hook_up_timer < 3.0f) {
        puffer->main_transform->position.z += elapsed * 30.0f;
        hook_up_timer += elapsed;
    }
    else{
        end();
    }
}

void QTE::end() {
    bait->is_active = true;
    bait->currently_in_qte = false;
    active = false;
}