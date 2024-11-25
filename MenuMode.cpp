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
	for (Button& button : current_menu_buttons)
		button.handle_event(e, window_size);



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
			while (selected < current_choices.size() && !current_choices[selected].on_select) --selected;
			if (selected >= current_choices.size()) selected = old;

			return true;
		} else if (e.key.keysym.sym == SDLK_DOWN && !mouse_select_mode_on) {
			//find next selectable thing that isn't selected:
			uint32_t old = selected;
			selected += 1;
			while (selected < current_choices.size() && !current_choices[selected].on_select) --selected;
			if (selected >= current_choices.size()) selected = old;
			return true;
		} else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
			if (selected < current_choices.size() && current_choices[selected].on_select) {
				current_choices[selected].on_select();
			}
			return true;
		}
	}
	return false;
}

void MenuMode::update(float elapsed) {
	if(is_game_over)
		menu_state = END_GAME;

	if(menu_state == IN_GAME && !in_game_menu_set) {
		end_game_menu_set = false;
		current_choices = pause_choices;
		current_menu_buttons = pause_menu_buttons;
		in_game_menu_set = true;
	} else if(menu_state == END_GAME && !end_game_menu_set) {
		in_game_menu_set = false;
		current_choices = end_choices;
		current_menu_buttons = end_menu_buttons;
		end_game_menu_set = true;
	}

	for (Button& button : current_menu_buttons)
		button.update(elapsed);

	if (background) {
		background->update(elapsed * (menu_state == BEFORE_START ? 0.7f : 0.1f));
	}
}

void MenuMode::draw(glm::uvec2 const &drawable_size) {
	if (background) {
		background->draw(drawable_size);
    }

    float y = (menu_state == IN_GAME ? 0.7f : 0.5f);

	uint32_t index = 0;

	for (auto const &choice : current_choices) {
		y -= choice.height;
		bool is_selected = (&choice - &current_choices[0] == selected);

		if(choice.is_button){
			if(current_menu_buttons[index].button_state == Button::ButtonState::Hover
				|| current_menu_buttons[index].button_state == Button::ButtonState::Pressing) 
			{
				mouse_select_mode_on = true;
				selected = index + 1;
				is_selected = true;
			}
			else{
				mouse_select_mode_on = false;
			}
			
			if(is_selected){
				current_menu_buttons[index].texture = font->get_text(">> " + choice.label + " <<");
			}
			else{
				current_menu_buttons[index].texture = font->get_text(choice.label);
			}

			current_menu_buttons[index].position = glm::vec2(0.5f, y);
			current_menu_buttons[index].draw(drawable_size);

			index++;
		}
		else{
			ui_render_program->draw_ui(*font->get_text(std::string(choice.label)), glm::vec2(0.5f, y), drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(1.0f), glm::vec3(0),true);
		}
	}
}
