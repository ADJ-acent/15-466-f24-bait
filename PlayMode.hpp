#pragma once
#include "Mode.hpp"
#include "QTEMode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"

#include <glm/glm.hpp>
#include "Puffer.hpp"
#include "Bait.hpp"
#include "BaitManager.hpp"

#include <vector>
#include <deque>

struct PlayMode : Mode {
	PlayMode();
	virtual ~PlayMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	//----- game state -----
	bool qte_active = false;

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, eat;

	glm::vec2 mouse_motion = glm::vec2(0);

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	Puffer puffer = Puffer();
	BaitManager bait_manager = BaitManager();

	std::shared_ptr< Puffer > shared_puffer_ptr;

	bool bait_in_eating_range = false;
	
	//camera:
	Scene::Camera *camera = nullptr;

	Texture cur_texture;

};
