#include <SDL.h>
#include "Scene.hpp"
#include "Bait.hpp"
#include "Puffer.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib> // For rand()

struct QTE {
    static int score;
    static std::vector< Bait > active_baits;

    bool active = false, success = false, failure = false, correct_key_pressed = false;            
    float timer = 0.0f, reel_up_timer = 0.0f, red_percentage = 0.0f;         
    float time_limit = 3.0f;    
    float input_delay = 0.0f;
    
    Puffer* puffer;
    Bait* bait;
    
    SDL_Keycode required_key;   

    // Possible keys for the QTE
    std::vector<SDL_Keycode> possible_keys = { SDLK_w, SDLK_a, SDLK_s, SDLK_d };

    QTE(Puffer *puffer_, Bait *bait_): puffer(puffer_), bait(bait_) {};
    QTE() = default;

    void start(); 
    void update(float elapsed); 
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