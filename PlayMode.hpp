#include "Mode.hpp"

#include "Scene.hpp"
#include "Sound.hpp"
#include "Collide.hpp"
#include "QTE.hpp"

#include <glm/glm.hpp>

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

	//local copy of the game scene (so code can change it during gameplay):
	Scene scene;

	Scene::Transform *fish = nullptr;
	Scene::Transform *rope = nullptr;
	Scene::Transform *bait = nullptr;

	Collider fish_collider;
	Collider rope_collider;
	Collider bait_collider;

	bool collide_with_bait;

	bool qte_active;

	QTE *eat_bait_QTE = nullptr;
	std::shared_ptr< Sound::PlayingSample > leg_tip_loop;
	
	//camera:
	Scene::Camera *camera = nullptr;

};
