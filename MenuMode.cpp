#include "MenuMode.hpp"

#include "Load.hpp"

#include "gl_compile_program.hpp"
#include "UIRenderProgram.hpp"

#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>

//---------- resources ------------

extern Load< UIRenderProgram > ui_render_program;
extern Load< Font > font;

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

//vao that binds nothing:
Load< GLuint > empty_binding(LoadTagDefault, [](){
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//empty vao has no attribute locations bound.
	glBindVertexArray(0);
	return new GLuint(vao);
});

//----------------------

bool MenuMode::handle_event(SDL_Event const &e, glm::uvec2 const &window_size) {
	if (e.type == SDL_KEYDOWN) {
		if (e.key.keysym.sym == SDLK_ESCAPE) {
			if (on_escape) {
				on_escape();
			} else {
				Mode::set_current(background);
			}
			return true;
		} else if (e.key.keysym.sym == SDLK_UP) {
			//find previous selectable thing that isn't selected:
			uint32_t old = selected;
			selected -= 1;
			while (selected < choices.size() && !choices[selected].on_select) --selected;
			if (selected >= choices.size()) selected = old;

			return true;
		} else if (e.key.keysym.sym == SDLK_DOWN) {
			//find next selectable thing that isn't selected:
			uint32_t old = selected;
			selected += 1;
			while (selected < choices.size() && !choices[selected].on_select) ++selected;
			if (selected >= choices.size()) selected = old;

			return true;
		} else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
			if (selected < choices.size() && choices[selected].on_select) {
				choices[selected].on_select();
			}
			return true;
		}
	}
	return false;
}

void MenuMode::update(float elapsed) {
	bounce += elapsed / 0.7f;
	bounce -= std::floor(bounce);

	if (background) {
		background->update(elapsed * background_time_scale);
	}
}

void MenuMode::draw(glm::uvec2 const &drawable_size) {
	if (background) {
		background->draw(drawable_size);
    }

    for (auto const &choice : choices) {
        ui_render_program->draw_ui(*font->get_text(std::string(choice.label)), glm::vec2(0.5f, 0.7f),drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(0.8f), glm::vec3(1),true);
    }
}
