#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "QTE.hpp"

#include <glm/glm.hpp>
#include "Puffer.hpp"
#include "Bait.hpp"

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

	//input tracking:
	struct Button {
		uint8_t downs = 0;
		uint8_t pressed = 0;
	} left, right, down, up, eat;

	glm::vec2 mouse_motion = glm::vec2(0);

	

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	Puffer puffer = Puffer();

	Scene::Transform *fish = nullptr;
	Scene::Transform *rope = nullptr;
	Scene::Transform *bait = nullptr;

	Collider fish_collider;
	Collider string_collider;
	Collider bait_collider;

	bool collide_with_bait;

	bool qte_active = false;

	QTE *eat_bait_QTE = nullptr;
	
	std::vector< Bait > active_bait;

	//camera:
	Scene::Camera *camera = nullptr;

};
