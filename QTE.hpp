// #include <SDL.h>

// #include <vector>
// #include <random>

// struct QTE {
// 	float qte_timer = 0.0f;
// 	float qte_time_limit = 3.0f;
//     SDL_Keycode qte_key = SDLK_q;
//     bool qte_success = false;
//     int qte_success_num = 0;

//     std::vector<SDL_Keycode> qte_keys = { SDLK_w, SDLK_a, SDLK_s, SDLK_d };
//     int random_index = ;

//     bool update(float elapsed);
// };

#include <SDL.h>

#include <vector>
#include <string>
#include <iostream>
#include <cstdlib> // For rand()

struct QTE {
    bool active = false;        
    bool success = false;       
    float timer;         
    float time_limit = 3.0f;    
    float input_delay;
    int success_count, success_count_goal;
    SDL_Keycode required_key;   

    // Possible keys for the QTE
    std::vector<SDL_Keycode> possible_keys = { SDLK_0, SDLK_1, SDLK_2, SDLK_3 };

    // Constructor
    QTE(){}

    void start(); 
    void update(float elapsed); 
    void reset();
    void end();

    // Draw the QTE text
    std::string get_prompt() {
        if (active) {
            return "Press '" + std::string(SDL_GetKeyName(required_key)) + "'! Time left: " + std::to_string(static_cast<int>(timer));
        }
        return "";
    }
};