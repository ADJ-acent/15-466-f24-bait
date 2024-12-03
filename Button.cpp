#include "Button.hpp"
#include <SDL.h>

extern Load< UIRenderProgram > ui_render_program;
extern Load< Sound::Sample > button_hover_sample;
extern Load< Sound::Sample > button_select_sample;
extern Load< Sound::Sample > hover1_sample;
extern Load< Sound::Sample > hover2_sample;
extern Load< Sound::Sample > hover3_sample;
extern Load< Sound::Sample > click1_sample;
extern Load< Sound::Sample > click2_sample;
extern Load< Sound::Sample > click3_sample;

Button::Button(Texture *texture_, glm::uvec2 padding_, glm::vec2 position_, glm::vec2 scale_, UIRenderProgram::AlignMode align_, glm::vec3 tint_, bool single_channel_,  std::function<void()> on_click_function_) :
texture(texture_), padding(padding_), position(position_), scale(scale_), align(align_), tint(tint_), single_channel(single_channel_), on_click_function(on_click_function_),
base_scale(scale_), base_tint(tint_)
{
}

void Button::set_hover_state(glm::vec2 scale_, glm::vec3 tint_)
{
    hover_scale = scale_;
    hover_tint = tint_;
}

void Button::set_pressing_state(glm::vec2 scale_, glm::vec3 tint_)
{
    pressing_scale = scale_;
    pressing_tint = tint_;
}

void Button::handle_event(SDL_Event const &event, glm::uvec2 const &window_size)
{
    if( event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP )
    {
        //Get mouse position
        int x = 0, y = 0;
        SDL_GetMouseState(&x, &y);
        // SDL has top left as origin while opengl has bottom left, change to opengl coordinate
        y = window_size.y - y;
        uint32_t ux = uint32_t(x);
        uint32_t uy = uint32_t(y);
        glm::u32vec4 bounding_box = get_bounding_box(window_size);
        uint32_t min_x = bounding_box.x;
        uint32_t min_y = bounding_box.y;
        uint32_t max_x = bounding_box.z;
        uint32_t max_y = bounding_box.w;
        //Check if mouse is in button
        bool in_button = !(ux < min_x || ux > max_x || uy < min_y || uy > max_y);
        ButtonState old_state = button_state;
        if (in_button) {
            // in button area
            if (button_state != Pressing) {
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    //just clicked the button
                    button_state = Pressing;
                }
                else {
                    //only hovering
                    button_state = Hover;
                }
            }
            else {
                if (event.type == SDL_MOUSEBUTTONUP) {
                    // pressed the button
                    if (on_click_function != nullptr) {
                        on_click_function();
                    }
                    button_state = Hover;
                }
            }
        }
        else {
            button_state = Base;
        }
    

        if (button_state != old_state) {
            state_switch_timer = 0.01f;
            button_sound_played = false; 
        }
    }
}

void Button::update(float elapsed)
{
    if (state_switch_timer != 0.0f) {
        state_switch_timer += elapsed;
        if (button_state == Button::ButtonState::Base) {
            float amt = std::min(state_switch_timer * base_animation_speed, 1.0f);
            tint = glm::mix(tint, base_tint, amt);
            scale = glm::mix(scale, base_scale, amt);
            if (amt == 1.0f) {
                state_switch_timer = 0.0f;
            }
        }
        else if (button_state == Button::ButtonState::Hover) {
            
            float amt = std::min(state_switch_timer * hover_animation_speed, 1.0f);
            if (hover_tint != glm::vec3(-1.0f)) {
                tint = glm::mix(tint, hover_tint, amt);
            }
            if (hover_scale != glm::vec2(-1.0f)) {
                scale = glm::mix(scale, hover_scale, amt);
            }
            if (amt == 1.0f) {
                state_switch_timer = 0.0f;
            }
            if(!button_sound_played){
                std::random_device rd;  // a seed source for the random number engine
                std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
                std::uniform_int_distribution<> distrib(1, 3); //number 1 2 3
                current_button_sound_number = distrib(gen);
                if(current_button_sound_number == 1){
                    button_hover_sound = Sound::play(*hover1_sample,2.0f);
                } else if(current_button_sound_number==2){
                    button_hover_sound = Sound::play(*hover2_sample,2.0f);
                } else if(current_button_sound_number==3){
                    button_hover_sound = Sound::play(*hover3_sample,2.0f);
                }
                // button_select_sound = Sound::play(*button_hover_sample,0.7f);
                
                button_sound_played = true;
            }

        }
        else if (button_state == Button::ButtonState::Pressing) {
            
            float amt = std::min(state_switch_timer * pressing_animation_speed, 1.0f);
            if (pressing_tint != glm::vec3(-1.0f)) {
                tint = glm::mix(tint, pressing_tint, amt);
            }
            if (pressing_scale != glm::vec2(-1.0f)) {
                scale = glm::mix(scale, pressing_scale, amt);
            }
            if (amt == 1.0f) {
                state_switch_timer = 0.0f;
            }
            if(!button_sound_played){
                if(current_button_sound_number == 1){
                    button_select_sound = Sound::play(*click1_sample,1.0f);
                } else if(current_button_sound_number==2){
                    button_select_sound = Sound::play(*click2_sample,1.0f);
                } else if(current_button_sound_number==3){
                    button_select_sound = Sound::play(*click3_sample,1.0f);
                }
                // button_select_sound = Sound::play(*button_select_sample,0.5f);
                button_sound_played = true;
            }

            
        }
    }
}

void Button::draw(glm::uvec2 drawable_size)
{
    ui_render_program->draw_ui(*texture, position, drawable_size, align, scale, tint, single_channel);
}

glm::u32vec4 Button::get_bounding_box(glm::uvec2 const &window_size) const
{
    float width = texture->width * scale.x;
	float height = texture->height * scale.y;

	glm::vec2 align_offset = UIRenderProgram::get_align_offset(align);
	align_offset.x *= width;
	align_offset.y *= height;

	float absolute_position_x = position.x * window_size.x + align_offset.x;
	float absolute_position_y = position.y * window_size.y + align_offset.y;

	return {absolute_position_x - padding.x, absolute_position_y - padding.y, absolute_position_x + width + padding.x, absolute_position_y + height + padding.y};
}
