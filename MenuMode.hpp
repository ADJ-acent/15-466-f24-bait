#pragma once

#include "Mode.hpp"
#include "Sound.hpp"

#include "gl_compile_program.hpp"
#include "UIRenderProgram.hpp"
#include "Font.hpp"
#include "Button.hpp"
#include "PlayMode.hpp"


#include <functional>
#include <vector>
#include <string>

extern Load< Font > font;
extern std::shared_ptr< PlayMode > play;
extern GameOverState game_over_state;

struct MenuMode : public Mode {
	enum MenuState {
		BEFORE_START,
		INSTRUCTION,
		IN_GAME,
		END_GAME
	};

	MenuMode() {
		//Start Menu set up
		start_choices.emplace_back("");
	
		start_choices.emplace_back("Play", [&](){
			menu_state = IN_GAME;
			SDL_SetRelativeMouseMode(SDL_TRUE);
			Mode::set_current(background);
		}, true);
		start_menu_buttons.push_back(start_choices.back().button);
		start_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		start_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		start_choices.emplace_back("Instruction", [&](){
			previous_menu_state = BEFORE_START;
			menu_state = INSTRUCTION;
		}, true);
		start_menu_buttons.push_back(start_choices.back().button);
		start_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		start_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		start_choices.emplace_back("Exit", [&](){
			Mode::set_current(nullptr);
		}, true);
		start_menu_buttons.push_back(start_choices.back().button);
		start_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		start_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		selected = 1;

		//Pause Menu set up
		pause_choices.emplace_back("");

		pause_choices.emplace_back("Resume", [&](){
			SDL_SetRelativeMouseMode(SDL_TRUE);
			play->puffer.velocity = play->puffer.pause_velocity;
			Mode::set_current(background);
		}, true);
		pause_menu_buttons.push_back(pause_choices.back().button);
		pause_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		pause_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		pause_choices.emplace_back("Restart", [&](){
			play.get()->bg_music_sound.get()->stop();
			play = std::make_shared< PlayMode >();
			QTE::score = 0;
			QTE::hunger = 100;
			game_over_state = NOT_OVER;
			menu_state = IN_GAME;
			SDL_SetRelativeMouseMode(SDL_TRUE);
			Mode::set_current(play);
		}, true);
		pause_menu_buttons.push_back(pause_choices.back().button);
		pause_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		pause_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		pause_choices.emplace_back("Instruction", [&](){
			previous_menu_state = IN_GAME;
			menu_state = INSTRUCTION;
		}, true);
		pause_menu_buttons.push_back(pause_choices.back().button);
		pause_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		pause_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		pause_choices.emplace_back("Exit", [&](){
			Mode::set_current(nullptr);
		}, true);
		pause_menu_buttons.push_back(pause_choices.back().button);
		pause_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		pause_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		//End Menu set up
		end_choices.emplace_back("");

		end_choices.emplace_back("Restart", [&](){
			play.get()->bg_music_sound.get()->stop();
			play = std::make_shared< PlayMode >();
			QTE::score = 0;
			QTE::hunger = 100;
			game_over_state = NOT_OVER;
			menu_state = IN_GAME;
			SDL_SetRelativeMouseMode(SDL_TRUE);
			Mode::set_current(play);
		}, true);
		end_menu_buttons.push_back(end_choices.back().button);
		end_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		end_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		end_choices.emplace_back("Exit", [&](){
			Mode::set_current(nullptr);
		}, true);
		end_menu_buttons.push_back(end_choices.back().button);
		end_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		end_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		//Instruction Menu set up
		instruction_choices.emplace_back("");

		instruction_choices.emplace_back("Back", [&](){
			menu_state = previous_menu_state;
		}, true);
		instruction_menu_buttons.push_back(instruction_choices.back().button);
		instruction_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
		instruction_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

		//Current Menu Set up
		current_choices = start_choices;
		current_menu_buttons = start_menu_buttons;
	}
	virtual ~MenuMode() { }

	virtual bool handle_event(SDL_Event const &event, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	struct Choice {
		Choice(std::string const &label_, std::function< void() > on_select_ = nullptr, bool is_button_ = false) : label(label_), on_select(on_select_), is_button(is_button_) { 
			button = Button(font->get_text(label),glm::uvec2(10,10), glm::vec2(0.5f), glm::vec2(1.0f), UIRenderProgram::AlignMode::Center, glm::vec3(0),true, on_select);
		}
		std::string label;
		std::function< void() > on_select;
		bool is_button;
		Button button;
        
		//height / padding give item height and padding relative to a screen of height 2:
		float height = 0.13f;
		float padding = 0.01f;
	};
	std::vector< Choice > start_choices;
	std::vector< Button > start_menu_buttons;

	std::vector< Choice > pause_choices;
	std::vector< Button > pause_menu_buttons;

	std::vector< Choice > end_choices;
	std::vector< Button > end_menu_buttons;

	std::vector< Choice > instruction_choices;
	std::vector< Button > instruction_menu_buttons;

	std::vector< Choice > current_choices;
	std::vector< Button > current_menu_buttons;

	MenuState previous_menu_state;

	
	uint32_t selected = 0;
	bool mouse_select_mode_on  = false;

	MenuState menu_state = BEFORE_START;
	bool in_game_menu_set = false;
	bool end_game_menu_set = false;
	bool instruction_menu_set = false;
	bool start_menu_set = true;

	//called when user presses 'escape':
	// (note: if not defined, menumode will Mode::set_current(background).)
	std::function< void() > on_escape;

	//will render this mode in the background if not null:
	std::shared_ptr< Mode > background;
};