#pragma once

#include "Texture.hpp"
#include "UIRenderProgram.hpp"
#include "Animation.hpp"
#include "Sound.hpp"
#include "GL.hpp"
#include <SDL.h>
#include <random>
#include <glm/glm.hpp>
#include <functional>

struct Button {
    Texture *texture;
    glm::uvec2 padding; // added to both side, actual width would be padding + width + padding
    glm::vec2 position;
    glm::vec2 scale;
    UIRenderProgram::AlignMode align;
    glm::vec3 tint;
    bool single_channel;
    bool button_sound_played = false;
    float state_switch_timer = 0.0f;

    enum ButtonState : uint8_t {
        Base = 0, // mouse outside button
        Hover = 1,
        Pressing = 2,
    } button_state = Base, last_state = Base;

    std::function<void()> on_click_function;

    glm::vec2 hover_scale = glm::vec2(-1.0f);
    glm::vec2 pressing_scale = glm::vec2(-1.0f);
    glm::vec2 base_scale;
    
    glm::vec3 hover_tint = glm::vec3(-1.0f);
    glm::vec3 pressing_tint = glm::vec3(-1.0f);
    glm::vec3 base_tint;
    static constexpr float base_animation_speed = 5.0f;
    static constexpr float hover_animation_speed = 5.0f;
    static constexpr float pressing_animation_speed = 5.0f;
    int current_button_sound_number = 0;

    Button(Texture *texture, glm::uvec2 padding, glm::vec2 position, glm::vec2 scale, UIRenderProgram::AlignMode align, glm::vec3 tint, bool single_channel,  std::function<void()> on_click_function);
    Button(){}

    void set_hover_state(glm::vec2 scale, glm::vec3 tint);
    void set_pressing_state(glm::vec2 scale, glm::vec3 tint);

    void handle_event(SDL_Event const &, glm::uvec2 const &window_size);

    void update(float elapsed);

    void draw(glm::uvec2 drawable_size);

    //return (bottom left x, y, top right x, y)
    glm::u32vec4 get_bounding_box(glm::uvec2 const &window_size) const;

	std::shared_ptr< Sound::PlayingSample > button_select_sound;
    std::shared_ptr< Sound::PlayingSample > button_hover_sound;
    std::shared_ptr< Sound::PlayingSample > hover1_sound;
    std::shared_ptr< Sound::PlayingSample > hover2_sound;
    std::shared_ptr< Sound::PlayingSample > hover3_sound;
};