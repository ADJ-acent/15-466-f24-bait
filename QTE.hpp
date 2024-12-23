#include <SDL.h>
#include "Scene.hpp"
#include "Bait.hpp"
#include "Puffer.hpp"
#include "math_helpers.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib> 

struct QTE {
    static int hunger;
    static int score;
    static std::vector< Bait > active_baits;

    bool active = false, success = false, failure = false, correct_key_pressed = false, trap_key_on = false, key_reset = false;     
    bool wrong_correct_played = false;       
    float timer = 0.0f, hook_up_timer = 0.0f, red_percentage = 0.0f;         
    float time_limit = 1.0f;    
    float input_delay, input_delay_time = 3.0f;

    int flashing_key_index = 0;

    float key_flash_reset_timer = 0.046f;
    
    Puffer* puffer;
    Bait* bait;
    
    SDL_Keycode required_key;   
    SDL_Keycode trap_key;

    // Possible keys for the QTE
    std::vector<SDL_Keycode> possible_keys = { SDLK_w, SDLK_a, SDLK_s, SDLK_d };
    std::vector<SDL_Keycode> trap_keys = possible_keys;

    std::shared_ptr< Sound::PlayingSample > timer_sound;
    std::shared_ptr< Sound::PlayingSample > flicker_sound;
    std::shared_ptr< Sound::PlayingSample > correct_sound;
    std::shared_ptr< Sound::PlayingSample > wrong_sound;
    std::shared_ptr< Sound::PlayingSample > congrats_sound;
    std::shared_ptr< Sound::PlayingSample > fail_sound;
    std::shared_ptr< Sound::PlayingSample > munch_sound;

    QTE(Puffer *puffer_, Bait *bait_): puffer(puffer_), bait(bait_) {};
    QTE() = default;

    void start(); 
    void update(float elapsed); 
    void key_flashing_reset();
    void reset();
    void bait_hook_up(float elapsed);
    void end();

    std::string get_prompt() {
        if (active) {
            return "Press '" + std::string(SDL_GetKeyName(required_key)) + "'! Time left: " + std::to_string(static_cast<int>(timer));
        }
        else if(success){
            return "QTE Success";
        }
        else if(failure){
            return "QTE Failed";
        }
        return "";
    }
};