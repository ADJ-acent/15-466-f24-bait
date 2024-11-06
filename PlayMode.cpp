#include "PlayMode.hpp"

#include "LitColorTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Texture.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "UIRenderProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint main_scene_for_lit_color_texture_program = 0;
GLuint puffer_scene_for_lit_color_texture_program = 0;
GLuint bait_scene_for_lit_color_texture_program = 0;
Load< MeshBuffer > main_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/ocean_scene.pnct"));
	main_scene_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< MeshBuffer > pufferfish_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/pufferfish.pnct"));
	puffer_scene_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< MeshBuffer > bait_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/bait_objects_2.pnct"));
	bait_scene_for_lit_color_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< Scene > main_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/ocean_scene.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = main_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = main_scene_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Scene > puffer_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/pufferfish.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = pufferfish_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = puffer_scene_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

Load< Scene > bait_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/bait_objects_2.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = bait_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = bait_scene_for_lit_color_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

extern UIElements ui_elements;
extern Load< UIRenderProgram > ui_render_program;

PlayMode::PlayMode() : scene(*main_scene) {

	std::vector<Scene::Transform *> puffer_transforms = scene.spawn(*puffer_scene,PUFFER);
	puffer.init(puffer_transforms);

	Bait bait_1 = Bait();
	std::vector<Scene::Transform *> bait_1_transforms = scene.spawn(*bait_scene,CARROT_BAIT);
	bait_1.init(bait_1_transforms, Circle);

	bait_manager.baits_in_use.emplace_back(bait_1);

	// Bait bait_2 = Bait();
	// std::vector<Scene::Transform *> bait_2_transforms = scene.spawn(*bait_scene,FISH_BAIT);
	// bait_2.init(bait_2_transforms, Square);

	// bait_manager.baits_in_use.emplace_back(bait_2);

	for(Bait b : bait_manager.baits_in_use){
    	
		if(b.type_of_bait == 0){
			b.string_collider = calculate_collider(b.mesh_parts.bait_string, bait_meshes->lookup("carrotbait_string"));
			b.bait_collider = calculate_collider(b.mesh_parts.bait_base, bait_meshes->lookup("carrotbait_base"));
		} else {
			b.string_collider = calculate_collider(b.mesh_parts.bait_string, bait_meshes->lookup("fishbait_string"));
			b.bait_collider = calculate_collider(b.mesh_parts.bait_base, bait_meshes->lookup("fishbait_base"));
		}

	}

	// puffer = scene.add_puffer(*puffer_scene);
	// puffer.init();
	//get pointer to camera for convenience:
	for (auto& cam : scene.cameras) {
		if (cam.transform->name == "PuffCam") {
			camera = &cam;
		}
	}
}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			return true;
		} else if (evt.key.keysym.sym == SDLK_a) {
			left.downs += 1;
			left.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.downs += 1;
			right.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.downs += 1;
			up.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.downs += 1;
			down.pressed = true;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			puffer.start_build_up();
			return true;
		} else if (evt.key.keysym.sym == SDLK_e) {
			eat.downs += 1;
			eat.pressed = true;
			return true;
		}
	} else if (evt.type == SDL_KEYUP) {
		if (evt.key.keysym.sym == SDLK_a) {
			left.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_d) {
			right.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_w) {
			up.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_s) {
			down.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_e) {
			eat.pressed = false;
			return true;
		} else if (evt.key.keysym.sym == SDLK_SPACE) {
			puffer.release();
		}
	} else if (evt.type == SDL_MOUSEBUTTONDOWN) {
		if (SDL_GetRelativeMouseMode() == SDL_FALSE) {
			SDL_SetRelativeMouseMode(SDL_TRUE);
			return true;
		}
	} else if (evt.type == SDL_MOUSEMOTION) {
		if (SDL_GetRelativeMouseMode() == SDL_TRUE) {
			mouse_motion = glm::vec2(
				evt.motion.xrel / float(window_size.y),
				evt.motion.yrel / float(window_size.y)
			);
			return true;
		}
	}

	return false;
}

void PlayMode::update(float elapsed) {
	int8_t swim_direction = int8_t(right.pressed) - int8_t(left.pressed);
	puffer.update(mouse_motion, swim_direction, elapsed);

	{		
		//check if there is bait in range
		bait_manager.check_bait_in_range(puffer.get_position(), puffer.get_forward());

		if (bait_manager.best_bait != nullptr && !qte_active) {
			bait_in_eating_range = true;

			if(eat.pressed) {
				eat.pressed = false;
				qte_active = true;
				
				shared_puffer_ptr = std::make_shared< Puffer >(puffer);

				QTEMode qte_mode = QTEMode(shared_puffer_ptr, bait_manager.best_bait);
				qte_mode.background = shared_from_this();
				Mode::set_current(std::make_shared< QTEMode >(qte_mode));
			}
		}
		else{
			bait_in_eating_range = false;
		}

		if(bait_manager.best_bait != nullptr){
			std::cout << "best_bait use count: " << bait_manager.best_bait.use_count() << std::endl;
		}

		bait_manager.bait_respawn(scene, bait_scene);

		// //respawn new bait if the bait is eaten up
		// if(shared_bait_ptr != nullptr && shared_bait_ptr->bait_bites_left == 0){
		// 	shared_bait_ptr = nullptr;
		// 	BaitManager::baits_in_use.pop_back();
		// 	Bait new_bait = Bait();
		// 	//pick either square or circle
		// 	std::srand(static_cast<unsigned int>(std::time(nullptr)));
		// 	auto circle_or_square = static_cast<BaitType>(std::rand() % 2);
		// 	std::vector<Scene::Transform *> new_bait_transforms;
		// 	if(circle_or_square==0){
		// 		new_bait_transforms = scene.spawn(*bait_scene, CARROT_BAIT);
		// 	} else {
		// 		new_bait_transforms = scene.spawn(*bait_scene, FISH_BAIT);
		// 	}
		// 	new_bait.init(new_bait_transforms, circle_or_square);
		// 	BaitManager::baits_in_use.push_back(new_bait);
		// 	new_bait.random_respawn_location();
		// } 
	}

	if(Mode::current == shared_from_this()){
		qte_active = false;
	}

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	mouse_motion = glm::vec2(0);
	eat.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for lit_color_texture_program:
	// TODO: consider using the Light(s) in the scene to do this
	glUseProgram(lit_color_texture_program->program);
	glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
	glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
	glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
	glUseProgram(0);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	ui_render_program->draw_ui(ui_elements.w, glm::vec2(0.5f),drawable_size);
	ui_render_program->draw_ui(ui_elements.w_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Center, glm::vec2(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));

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

		if(bait_in_eating_range && !qte_active){
			lines.draw_text("Press E to eat the bait",
				glm::vec3(-aspect + 2.0f * H, -1.0 + 2.0f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text("Press E to eat the bait",
				glm::vec3(-aspect + 2.0f * H + ofs, -1.0 + + 2.0f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff, 0xff, 0xff, 0x00));
		}

		lines.draw_text("Score: " + std::to_string(QTE::score),
			glm::vec3(-aspect + 0.1f * H, -1.0 + 5.0f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
        lines.draw_text("Score: " + std::to_string(QTE::score),
            glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 5.0f * H + ofs, 0.0),
            glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
            glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}

	GL_ERRORS();
}