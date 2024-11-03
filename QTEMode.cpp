#include "QTEMode.hpp"

#include "Load.hpp"
#include "DrawLines.hpp"
#include "gl_compile_program.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

GLint fade_program_color = -1;

Load< GLuint > fade_program(LoadTagEarly, [](){
	GLuint *ret = new GLuint(gl_compile_program(
		"#version 330\n"
		"void main() {\n"
		"	gl_Position = vec4(4 * (gl_VertexID & 1) - 1,  2 * (gl_VertexID & 2) - 1, 0.0, 1.0);\n"
		"}\n"
	,
		"#version 330\n"
		"uniform vec4 color;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	fragColor = color;\n"
		"}\n"
	));

	fade_program_color = glGetUniformLocation(*ret, "color");

	return ret;
});

//-------------------------------------

QTEMode::QTEMode(std::shared_ptr< Puffer > puffer, std::shared_ptr< Bait > bait) {
    eat_bait_QTE = QTE(puffer, bait);
    eat_bait_QTE.start();
}

bool QTEMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
            eat_bait_QTE.end();
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

    eat_bait_QTE.update(elapsed);
    if(!eat_bait_QTE.active){
        Mode::set_current(background);
    }
}

void QTEMode::draw(glm::uvec2 const &drawable_size) {
	if (background && background_fade < 1.0f) {
	    background->draw(drawable_size);

	// 	glDisable(GL_DEPTH_TEST);
	// 	if (background_fade > 0.0f) {
	// 		glEnable(GL_BLEND);
	// 		glBlendEquation(GL_FUNC_ADD);
	// 		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// 		glUseProgram(*fade_program);
	// 		glUniform4fv(fade_program_color, 1, glm::value_ptr(glm::vec4(0.0f, 0.0f, 0.0f, background_fade)));
	// 		glDrawArrays(GL_TRIANGLES, 0, 3);
	// 		glUseProgram(0);
	// 		glDisable(GL_BLEND);
	// 	}
	}
	// glDisable(GL_DEPTH_TEST);

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

		if(eat_bait_QTE.active && eat_bait_QTE.input_delay <= 0 && !eat_bait_QTE.failure){
			lines.draw_text(eat_bait_QTE.get_prompt(),
				glm::vec3(-aspect + 2.0f * H, -1.0 + 2.0f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text(eat_bait_QTE.get_prompt(),
				glm::vec3(-aspect + 2.0f * H + ofs, -1.0 + + 2.0f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff * (eat_bait_QTE.red_text_percentage / eat_bait_QTE.time_limit), 
							0x00, 
							0x00, 0x00));
		}
		else if(eat_bait_QTE.failure){
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