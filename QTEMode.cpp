#include "QTEMode.hpp"

#include "Load.hpp"
#include "DrawLines.hpp"
#include "gl_compile_program.hpp"
#include "UIRenderProgram.hpp"
#include "Animation.hpp"
#include "Font.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

extern SpriteAnimation qte_timer_animation;
extern Load< UIRenderProgram > ui_render_program;
extern Load< Font > font;

//-------------------------------------

QTEMode::QTEMode(Puffer *puffer, Bait *bait) {
    std::cout << "eat_bait_QTE starts" << std::endl;
	eat_bait_QTE = new QTE(puffer, bait);
    eat_bait_QTE->start();
}

bool QTEMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
            eat_bait_QTE->end();
            Mode::set_current(background);
			return true;
		} 
    }
	return false;
}

void QTEMode::update(float elapsed) {
    if(background){
        background->update(elapsed * background_time_scale);
    }

	cur_texture = qte_timer_animation.get_current_frame(eat_bait_QTE->timer/eat_bait_QTE->time_limit);

    eat_bait_QTE->update(elapsed);
    if(!eat_bait_QTE->active){
		if(eat_bait_QTE->failure){
			QTE::score = 100;
		}

        Mode::set_current(background);
    }
}

void QTEMode::draw(glm::uvec2 const &drawable_size) {
	if (background && background_fade < 1.0f) {
	    background->draw(drawable_size);
	}
 
	if(eat_bait_QTE->active && !eat_bait_QTE->failure) {
		if(eat_bait_QTE->input_delay <= 0.0f){
			if(eat_bait_QTE->correct_key_pressed) {
				ui_render_program->draw_ui(keys_ui[eat_bait_QTE->required_key], glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			}
			else if(eat_bait_QTE->trap_key_on){
				ui_render_program->draw_ui(keys_ui[eat_bait_QTE->trap_key], glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(0.0f));
			}
			else{
				ui_render_program->draw_ui(keys_ui[eat_bait_QTE->required_key], glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
				ui_render_program->draw_ui(cur_texture, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
			}
		}
		else{
			ui_render_program->draw_ui(keys_ui[eat_bait_QTE->required_key], glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f, 1.0f));
		}
	}
	else if(eat_bait_QTE->failure) {
		ui_render_program->draw_ui(*font->get_text(std::string("Baited!!! FINAL HUNGER: " + std::to_string(QTE::score))), glm::vec2(0.5f, 0.7f),drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(0.8f), glm::vec3(1),true);
	}

}