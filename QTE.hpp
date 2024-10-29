#include <SDL.h>
#include "Scene.hpp"
#include "Bait.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib> // For rand()

struct QTE {
    static int score;
    static std::vector< Bait > active_baits;

    bool active = false, success = false, failure = false, respawn_new_bait = false;            
    float timer, hook_up_timer;         
    float time_limit = 3.0f;    
    float input_delay;
    int success_count, success_count_goal;
    

    Scene::Transform *fish = nullptr;
    Scene::Transform *string = nullptr;
    Scene::Transform *bait = nullptr;

    SDL_Keycode required_key;   

    // Possible keys for the QTE
    std::vector<SDL_Keycode> possible_keys = { SDLK_0, SDLK_1, SDLK_2, SDLK_3 };

    // Constructor with arguments
    QTE(Scene::Transform *fish, Scene::Transform *string, Scene::Transform *bait){
        this->fish = fish;
        this->string = string;
        this->bait = bait;
    }
    
    // Constructor without argument
    QTE(){}

    void start(int goal); 
    void update(float elapsed); 
    void reset();
    void bait_hook_up(float elapsed);
    void bait_eaten();
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