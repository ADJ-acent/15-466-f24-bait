#pragma once
#include "Mode.hpp"
#include "QTE.hpp"
#include "PlayMode.hpp"

#include <functional>
#include <vector>
#include <string>
#include <cstdlib>

extern UIElements ui_elements;

struct QTEMode : public Mode {
    QTEMode(Puffer *puffer, Bait *bait);
	virtual ~QTEMode() { }

	virtual bool handle_event(SDL_Event const &event, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, quit;

	std::unordered_map<SDL_Keycode, Texture> keys_ui = {
		{SDLK_w, ui_elements.w_pressed},
		{SDLK_a, ui_elements.a_pressed},
		{SDLK_s, ui_elements.s_pressed},
		{SDLK_d, ui_elements.d_pressed}
	};

    QTE *eat_bait_QTE;
	
	Texture cur_texture;

	//will render this mode in the background if not null:
	std::shared_ptr< Mode > background;
	// float background_time_scale = 1.0f;
	// float background_fade = 0.5f;
};
