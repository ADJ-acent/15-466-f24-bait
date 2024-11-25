#include "QTE.hpp"

int QTE::hunger = 100;
int QTE::score = 0;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<int> dist_index(0, 3);
std::uniform_int_distribution<int> dist_trap_index(0, 2);

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
    puffer->qte_enter(bait);
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
            key_flashing_reset();
            if(input_delay > 1.5f) {
                key_flash_reset_timer = 0.046f;
            }
            else if(input_delay > 0.8f){
                key_flash_reset_timer = 0.092f;
            }
            else{
                key_flash_reset_timer = 0.28f;
            }
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
            QTE::hunger++; 
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
            if(!trap_key_on){
                std::cout << "QTE Failed! Time's up!" << std::endl;
                failure = true;
                return;
            }

            input_delay = input_delay_time;
            reset();
            key_reset = false;
            return;
        }
    }
    else{
        if(timer >= time_limit && !success){
            input_delay = input_delay_time;
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
    key_flash_reset_timer = 0.046f;
    trap_keys = possible_keys;

    int random_index = dist_index(gen);
    required_key = possible_keys[random_index];
    int random_trap_index = dist_index(gen);

    if(random_trap_index == random_index){
        trap_key_on = true;
        trap_keys.erase(trap_keys.begin() + random_index);
        random_trap_index = dist_trap_index(gen);
        trap_key = trap_keys[random_trap_index];

        std::cout << "trap key is on" << std::endl;
        std::cout << trap_key << std::endl;
    }
    else{
        trap_key_on = false;
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
    puffer->qte_death(bait);
    if(hook_up_timer < 3.0f) {
        puffer->main_transform->position.z += elapsed * 30.0f;
        hook_up_timer += elapsed;
    }
    else{
        end();
    }
}

void QTE::end() {
    puffer->qte_exit();
    bait->to_siberia();
    bait->is_active = true;
    bait->currently_in_qte = false;
    active = false;
}