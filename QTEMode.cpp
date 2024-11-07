#include "QTEMode.hpp"

#include "Load.hpp"
#include "DrawLines.hpp"
#include "gl_compile_program.hpp"
#include "UIRenderProgram.hpp"
#include "Animation.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

extern SpriteAnimation qte_timer_animation;
extern UIElements ui_elements;
extern Load< UIRenderProgram > ui_render_program;

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
		// if(eat_bait_QTE->failure){
		// 	Mode::set_current(std::make_shared< PlayMode >());
		// }

        Mode::set_current(background);
    }
}

void QTEMode::draw(glm::uvec2 const &drawable_size) {
	if (background && background_fade < 1.0f) {
	    background->draw(drawable_size);
	}

	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		constexpr float H = 0.3f;

		if(eat_bait_QTE->active && !eat_bait_QTE->failure) {
			if(eat_bait_QTE->required_key == SDLK_w){
				if(eat_bait_QTE->correct_key_pressed) {
					ui_render_program->draw_ui(ui_elements.w_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else{
					ui_render_program->draw_ui(ui_elements.w_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
					ui_render_program->draw_ui(cur_texture, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
				}
			}
			else if(eat_bait_QTE->required_key == SDLK_a){
				if(eat_bait_QTE->correct_key_pressed) {
					ui_render_program->draw_ui(ui_elements.a_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else{
					ui_render_program->draw_ui(ui_elements.a_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
					ui_render_program->draw_ui(cur_texture, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
				}
			}
			else if(eat_bait_QTE->required_key == SDLK_s){
				if(eat_bait_QTE->correct_key_pressed) {
					ui_render_program->draw_ui(ui_elements.s_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else{
					ui_render_program->draw_ui(ui_elements.s_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
					ui_render_program->draw_ui(cur_texture, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
				}
			}
			else if(eat_bait_QTE->required_key == SDLK_d){
				if(eat_bait_QTE->correct_key_pressed) {
					ui_render_program->draw_ui(ui_elements.d_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				}
				else{
					ui_render_program->draw_ui(ui_elements.d_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
					ui_render_program->draw_ui(cur_texture, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Top, glm::vec2(3.0f), glm::vec3(1.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f, 1.0f - eat_bait_QTE->red_percentage * 2.0f));
				}
			}
		}
		else if(eat_bait_QTE->failure) {
			lines.draw_text("Baited!!! FINAL SCORE: " + std::to_string(QTE::score),
				glm::vec3(-aspect + 2.0f * H, -1.0 + 2.0f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text("Baited!!! FINAL SCORE: " + std::to_string(QTE::score),
				glm::vec3(-aspect + 2.0f * H + ofs, -1.0 + + 2.0f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		}
	}
}