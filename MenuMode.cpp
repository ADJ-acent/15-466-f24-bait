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
	if(is_before_game_start){
		for (Button& button : start_menu_buttons)
			button.handle_event(e, window_size);
	}
	else{
		for (Button& button : pause_menu_buttons)
			button.handle_event(e, window_size);
	}

	if (e.type == SDL_KEYDOWN) {
		if (e.key.keysym.sym == SDLK_ESCAPE) {
			if (on_escape) {
				on_escape();
			} else {
				Mode::set_current(background);
			}
			return true;
		} else if (e.key.keysym.sym == SDLK_UP && !mouse_select_mode_on) {
			//find previous selectable thing that isn't selected:
			uint32_t old = selected;
			selected -= 1;
			if(is_before_game_start){
				while (selected < start_choices.size() && !start_choices[selected].on_select) --selected;
				if (selected >= start_choices.size()) selected = old;
			}
			else{
				while (selected < pause_choices.size() && !pause_choices[selected].on_select) --selected;
				if (selected >= pause_choices.size()) selected = old;
			}

			return true;
		} else if (e.key.keysym.sym == SDLK_DOWN && !mouse_select_mode_on) {
			//find next selectable thing that isn't selected:
			uint32_t old = selected;
			selected += 1;
			if(is_before_game_start){
				while (selected < start_choices.size() && !start_choices[selected].on_select) --selected;
				if (selected >= start_choices.size()) selected = old;
			}
			else{
				while (selected < pause_choices.size() && !pause_choices[selected].on_select) --selected;
				if (selected >= pause_choices.size()) selected = old;
			}

			return true;
		} else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
			if(is_before_game_start){
				if (selected < start_choices.size() && start_choices[selected].on_select) {
					start_choices[selected].on_select();
				}
			}
			else{
				if (selected < pause_choices.size() && pause_choices[selected].on_select) {
					pause_choices[selected].on_select();
				}
			}
			return true;
		}
	}
	return false;
}

void MenuMode::update(float elapsed) {
	if(is_before_game_start){
		for (Button& button : start_menu_buttons)
			button.update(elapsed);
	}
	else{
		for (Button& button : pause_menu_buttons)
			button.update(elapsed);
	}

	if (background) {
		background->update(elapsed * (is_before_game_start ? 0.7f : 0.1f));
	}

	std::cout << selected << std::endl;
}

void MenuMode::draw(glm::uvec2 const &drawable_size) {
	if (background) {
		background->draw(drawable_size);
    }

    float y = 0.7f;

	uint32_t index = 0;
	if(is_before_game_start){
		for (auto const &choice : start_choices) {
			y -= choice.height;
			bool is_selected = (&choice - &start_choices[0] == selected);

			if(choice.is_button){
				if(start_menu_buttons[index].button_state == Button::ButtonState::Hover
					|| start_menu_buttons[index].button_state == Button::ButtonState::Pressing) 
				{
					mouse_select_mode_on = true;
					selected = index + 1;
					is_selected = true;
				}
				else{
					mouse_select_mode_on = false;
				}
				
				if(is_selected){
					start_menu_buttons[index].texture = font->get_text(">> " + choice.label + " <<");
				}
				else{
					start_menu_buttons[index].texture = font->get_text(choice.label);
				}

				start_menu_buttons[index].position = glm::vec2(0.5f, y);
				start_menu_buttons[index].draw(drawable_size);

				index++;
			}
			else{
				ui_render_program->draw_ui(*font->get_text(std::string(choice.label)), glm::vec2(0.5f, y), drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(1.0f), glm::vec3(0),true);
			}
		}
	}
	else{
		for (auto const &choice : pause_choices) {
			y -= choice.height;
			bool is_selected = (&choice - &pause_choices[0] == selected);

			if(choice.is_button){
				if(pause_menu_buttons[index].button_state == Button::ButtonState::Hover
					|| pause_menu_buttons[index].button_state == Button::ButtonState::Pressing) 
				{
					mouse_select_mode_on = true;
					selected = index + 1;
					is_selected = true;
				}
				else{
					mouse_select_mode_on = false;
				}
				
				if(is_selected){
					pause_menu_buttons[index].texture = font->get_text(">> " + choice.label + " <<");
				}
				else{
					pause_menu_buttons[index].texture = font->get_text(choice.label);
				}

				pause_menu_buttons[index].position = glm::vec2(0.5f, y);
				pause_menu_buttons[index].draw(drawable_size);

				index++;
			}
			else{
				ui_render_program->draw_ui(*font->get_text(std::string(choice.label)), glm::vec2(0.5f, y), drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(1.0f), glm::vec3(0),true);
			}
		}
	}
}
