#include "PlayMode.hpp"
#include "MenuMode.hpp"

#include "DepthTextureProgram.hpp"
#include "LitColorTextureProgram.hpp"
#include "WaveTextureProgram.hpp"
#include "WiggleTextureProgram.hpp"
#include "TransTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Texture.hpp"
#include "UIRenderProgram.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Framebuffers.hpp"
#include "GameConfig.hpp"
#include "Font.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

extern std::shared_ptr< MenuMode > menu;
bool is_game_over = false;

GLuint main_scene_for_depth_texture_program = 0;
GLuint puffer_scene_for_depth_texture_program = 0;
GLuint bait_scene_for_depth_texture_program = 0;
GLuint chopping_board_scene_for_depth_texture_program = 0;
GLuint waterplane_scene_for_wave_texture_program = 0;
GLuint seaweed_objs_for_wiggle_texture_program = 0;
GLuint wall_objs_for_trans_texture_program = 0;



Scene::Drawable *waterplane_drawable = nullptr;

Load< MeshBuffer > main_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/ocean_scene.pnct"));
	main_scene_for_depth_texture_program = ret->make_vao_for_program(depth_texture_program->program);
	seaweed_objs_for_wiggle_texture_program = ret->make_vao_for_program(wiggle_texture_program->program);
	waterplane_scene_for_wave_texture_program = ret->make_vao_for_program(wave_texture_program->program);
	wall_objs_for_trans_texture_program = ret->make_vao_for_program(trans_texture_program->program);
	return ret;
});

Load< MeshBuffer > pufferfish_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/pufferfish.pnct"));
	puffer_scene_for_depth_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load< MeshBuffer > bait_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/bait_objects.pnct"));
	bait_scene_for_depth_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});

Load<MeshBuffer> chopping_board_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/chopping_board.pnct"));
	chopping_board_scene_for_depth_texture_program = ret->make_vao_for_program(lit_color_texture_program->program);
	return ret;
});


Load< Scene > main_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/ocean_scene.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = main_meshes->lookup(mesh_name);

		
		if(mesh_name.find("invisible") == -1)
		{
			if(mesh_name.find("seaweed") != -1)
			{
				scene.drawables.emplace_back(transform);
				Scene::Drawable &drawable = scene.drawables.back();
				drawable.pipeline = wiggle_texture_program_pipeline;
				drawable.pipeline.vao = seaweed_objs_for_wiggle_texture_program;
				drawable.pipeline.type = mesh.type;
				drawable.pipeline.start = mesh.start;
				drawable.pipeline.count = mesh.count;
						
			}
			else if(mesh_name.find("wall") != -1)
			{
				scene.drawables.emplace_back(transform);
				Scene::Drawable &drawable = scene.drawables.back();
				drawable.pipeline = trans_texture_program_pipeline;
				drawable.pipeline.vao = wall_objs_for_trans_texture_program;
				drawable.pipeline.type = mesh.type;
				drawable.pipeline.start = mesh.start;
				drawable.pipeline.count = mesh.count;
			}
			else if(mesh_name.find("sand") != -1)
			{
				scene.drawables.emplace_back(transform);
				Scene::Drawable &drawable = scene.drawables.back();
				drawable.pipeline = depth_texture_program_pipeline;
				drawable.pipeline.vao = main_scene_for_depth_texture_program;
				drawable.pipeline.type = mesh.type;
				drawable.pipeline.start = mesh.start;
				drawable.pipeline.count = mesh.count;
			}
			else if(mesh_name.find("waterplane") != -1 || mesh_name.find("puddle") != -1)
			{
				scene.drawables.emplace_back(transform);
				Scene::Drawable &drawable = scene.drawables.back();
				//drawable.hidden = true;
				//waterplane_drawable = &scene.drawables.back();
				drawable.pipeline = wave_texture_program_pipeline;
				drawable.pipeline.vao = waterplane_scene_for_wave_texture_program;
				drawable.pipeline.type = mesh.type;
				drawable.pipeline.start = mesh.start;
				drawable.pipeline.count = mesh.count;
				
				
			}
			else
			{
				scene.drawables.emplace_back(transform);
				Scene::Drawable &drawable = scene.drawables.back();
				drawable.pipeline = lit_color_texture_program_pipeline;
				drawable.pipeline.vao = main_scene_for_depth_texture_program;
				drawable.pipeline.type = mesh.type;
				drawable.pipeline.start = mesh.start;
				drawable.pipeline.count = mesh.count;
			}

		Scene::Drawable &drawable = scene.drawables.back();
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
		drawable.mesh = &mesh;
		drawable.meshbuffer = &(*main_meshes);
		
		}
		//NOTE: add this to other scenes

	});
});

Load< Scene > puffer_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/pufferfish.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = pufferfish_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = puffer_scene_for_depth_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;
		drawable.mesh = &mesh;
		drawable.meshbuffer = &(*pufferfish_meshes);

	});
});

Load< Scene > bait_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/bait_objects.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = bait_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = bait_scene_for_depth_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

		drawable.mesh = &mesh;
		drawable.meshbuffer = &(*bait_meshes);
	});
});

Load< Scene > chopping_board_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/chopping_board.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = chopping_board_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = lit_color_texture_program_pipeline;

		drawable.pipeline.vao = chopping_board_scene_for_depth_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

		drawable.mesh = &mesh;
		drawable.meshbuffer = &(*chopping_board_meshes);
	});
});

// noise samples
Load< Sound::Sample >  flipper_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/flipper.wav"));
});

Load< Sound::Sample >  through_water_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/through_water.wav"));
});

Load< Sound::Sample >  blow_up_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/charge_up.wav"));
});

Load< Sound::Sample >  whoosh_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/whoosh.wav"));
});

Load< Sound::Sample >  bump_1_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/bump_1.wav"));
});

Load< Sound::Sample >  button_select_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/button_select.wav"));
});

Load< Sound::Sample >  button_hover_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/button_hover.wav"));
});

Load< Sound::Sample >  timer_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/timer_tick.wav"));
});

Load< Sound::Sample >  flicker_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/flicker.wav"));
});

Load< Sound::Sample >  correct_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/correct.wav"));
});

Load< Sound::Sample >  wrong_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/wrong.wav"));
});

Load< Sound::Sample >  bg_music_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/bg_music.wav"));
});

Load< Sound::Sample >  congrats_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/congrats.wav"));
});

Load< Sound::Sample >  fail_sample(LoadTagDefault, []() -> Sound::Sample const * {
	return new Sound::Sample(data_path("sound/fail.wav"));
});

extern UIElements ui_elements;
extern Load< UIRenderProgram > ui_render_program;
extern Load< Font > font;

GameConfig game_config;

PlayMode::PlayMode() : scene(*main_scene) {

	bg_music_sound = Sound::loop(*bg_music_sample,0.2f);


	std::vector<Scene::Transform *> puffer_transforms = scene.spawn(*puffer_scene,PUFFER);

	puffer.init(puffer_transforms, &scene, &particle_system);

	for(int i = 0; i < 4; i++){
		Bait new_bait = Bait();
		std::vector<Scene::Transform *> new_bait_transforms = scene.spawn(*bait_scene,CARROT_BAIT);
		new_bait.init(new_bait_transforms, CIRCLE);
		new_bait.random_respawn_location();
		bait_manager.baits_in_use.push_back(new_bait);
		bait_manager.active_baits_num++;
	}

	for(int i = 0; i < 4; i++){
		Bait new_bait = Bait();
		std::vector<Scene::Transform *> new_bait_transforms = scene.spawn(*bait_scene,FISH_BAIT);
		new_bait.init(new_bait_transforms, SQUARE);
		new_bait.random_respawn_location();
		bait_manager.baits_in_use.push_back(new_bait);
		bait_manager.active_baits_num++;
	}

	std::vector<Scene::Transform *> chopping_board_transforms = scene.spawn(*chopping_board_scene,CHOPPING_BOARD);
	for (auto t : chopping_board_transforms){
        if (t->name == "choppingboard_main") {
			chopping_board_main_mesh = t;
			chopping_board_main_mesh->position = glm::vec3(0.0f, 0.0f, 200.0f);
			chopping_board_main_mesh->scale = glm::vec3(0.0f);
        }
	}

	//get pointer to camera for convenience:
	for (auto& cam : scene.cameras) {
		if (cam.transform->name == "PuffCam") {
			camera = &cam;
		}
	}

	particle_system.active_camera = &camera;
	for (auto& transform : scene.transforms) {
		if (transform.name.find("waterplane") != -1) {
			Scene::Transform*temp = &transform;
			waterplane_size = temp;
			waterheight = temp->position.z;
		}
	}

	for (auto &transform: scene.transforms)
	{
		if(transform.name.find("invisible_duck") != -1)
		{rotate_duck = &transform;}
		else if(transform.name.find("invisible_boat") != -1)
		{rotate_boat = &transform;}
	}

}

PlayMode::~PlayMode() {
}

bool PlayMode::handle_event(SDL_Event const &evt, glm::uvec2 const &window_size) {
	//example of setting up a button in the center of the screen, please remove when needed along with example_buttons field in playmode.hpp
	// for (Button& button : example_buttons)
	// 	button.handle_event(evt, window_size);

	if (evt.type == SDL_KEYDOWN) {
		if (evt.key.keysym.sym == SDLK_ESCAPE) {
			SDL_SetRelativeMouseMode(SDL_FALSE);
			puffer.pause_velocity = puffer.velocity;
			menu->background = shared_from_this();
			Mode::set_current(menu);
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
		} else if (evt.key.keysym.sym == SDLK_0) {
			debug.downs += 1;
			debug.pressed = true;
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
		} else if (evt.key.keysym.sym == SDLK_0) {
			debug.pressed = false;
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
	//example of setting up a button in the center of the screen, please remove when needed along with example_buttons field in playmode.hpp

	// for (Button& button : example_buttons)
	// 	button.update(elapsed);

	if (debug.downs != 0) {
		game_config.charge_face_camera = !game_config.charge_face_camera;
	}
	
	elapsedtime += elapsed;

	rotate_duck->rotation = rotate_duck->rotation * glm::angleAxis(glm::radians(0.1f), glm::vec3(0.0f, 0.0f, -1.0f));

	rotate_boat->rotation = rotate_boat->rotation * glm::angleAxis(glm::radians(0.05f), glm::vec3(0.0f, 0.0f, 1.0f));

	int8_t swim_direction = int8_t(right.pressed) - int8_t(left.pressed);
	puffer.update(mouse_motion, swim_direction, elapsed);
	particle_system.update(elapsed);
	bait_manager.update_bait_lifetime(elapsed);
	{		
		//check if there is bait in range
		bait_manager.check_bait_in_range(puffer.get_position(), puffer.get_forward());

		if (bait_manager.best_bait_index >= 0 
		&& bait_manager.baits_in_use[bait_manager.best_bait_index].is_active 
		&& !qte_active && bait_manager.baits_in_use[bait_manager.best_bait_index].bait_bites_left > 0) 
		{
			bait_in_eating_range = true;

			if(eat.pressed) {
				eat.pressed = false;
				qte_active = true;

				// reset keys to prevent sticking (remain pressed during qte)
				left.pressed = false;
				right.pressed = false;
				up.pressed = false;
				down.pressed = false;

				QTEMode qte_mode = QTEMode(&puffer, &bait_manager.baits_in_use[bait_manager.best_bait_index]);
				qte_mode.background = shared_from_this();
				Mode::set_current(std::make_shared< QTEMode >(qte_mode));
			}
		}
		else {
			bait_in_eating_range = false;
		}
	}

	if(Mode::current == shared_from_this()){
		qte_active = false;
	}


	if(Mode::current == menu && menu->menu_state == MenuMode::BEFORE_START){
		puffer.switch_to_main_menu_camera();
	}
	
	if(Mode::current == menu && menu->menu_state == MenuMode::IN_GAME){
		puffer.velocity = glm::vec3(0.0f);
	}

	hunger_decrement_counter += elapsed;
    if(hunger_decrement_counter > 5.0f){
        hunger_decrement_counter = 0.0f;
        QTE::hunger -= 1;
	}

	if(is_game_over){
		chopping_board_main_mesh->scale = glm::vec3(1.0f);
		puffer.main_transform->rotation = puffer.original_rotation;
		puffer.camera->position = glm::vec3(0.0f, -30.0f, 210.0f);
		puffer.main_transform->position = glm::vec3(0.0f, 0.0f, 205.0f);

		
		wobble += elapsed / 1.0f;
		wobble -= std::floor(wobble);
		for(Scene::Transform* collectible : puffer.collected){
			
			collectible->rotation = collectible->rotation * glm::angleAxis(
				glm::radians(5.0f * std::sin(wobble * 2.0f * float(M_PI))),
				glm::vec3(0.0f, 0.2f, 0.0f)
			);
		}
		
		SDL_SetRelativeMouseMode(SDL_FALSE);
		Mode::set_current(menu);
	}

	

	//reset button press counters:
	left.downs = 0;
	right.downs = 0;
	up.downs = 0;
	down.downs = 0;
	mouse_motion = glm::vec2(0);
	eat.downs = 0;
	debug.downs = 0;
}

void PlayMode::draw(glm::uvec2 const &drawable_size) {
	//update camera aspect ratio for drawable:
	framebuffers.realloc(drawable_size);
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	glm::vec2 waterheight_direction = glm::vec2(waterheight,1.0f);
	glm::vec4 xyzvec = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 scale = waterplane_size->scale; 

	//set up light type and position for depth_texture_program:
	// all the shaders

	{	
		glUseProgram(depth_texture_program->program);
		glUniform1i(depth_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(depth_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
		glUniform3fv(depth_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform1f(depth_texture_program->TIME_float, elapsedtime);
		glUseProgram(0);

		glUseProgram(trans_texture_program->program);
		glUniform1i(trans_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(trans_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
		glUniform3fv(trans_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUseProgram(0);
		

		glUseProgram(wiggle_texture_program->program);
		glUniform1i(wiggle_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(wiggle_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
		glUniform3fv(wiggle_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform1f(wiggle_texture_program->TIME_float, elapsedtime);
		glUniform3fv(wiggle_texture_program->PLAYERPOS_vec3, 1, glm::value_ptr( puffer.get_position()));
		glUniform1f(wiggle_texture_program->PLAYERSCALE_float, puffer.current_scale);
		glUseProgram(0);
		
	}

	
	// WRITE TO REFRACT FRAME BUFFER
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.refract_fb);
		waterheight_direction.y = 1.0f;
		//waterplane_drawable->hidden = false;
		
		waterplane_size->scale = glm::vec3(0.0f);

		glUseProgram(lit_color_texture_program->program);
		glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f,1.0f)));
		glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform2fv(lit_color_texture_program->WATER_HEIGHT_DIRECTION_vec2, 1, glm::value_ptr(waterheight_direction));
		glUniform3fv(lit_color_texture_program->CAMPOS_vec3, 1, glm::value_ptr( camera->transform->make_local_to_world() * xyzvec));
		glUseProgram(0);

		glClearColor(0.0f, 2.0f, 8.0f, 1.0f);
		glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CLIP_DISTANCE0);
		glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.
		scene.draw(*camera);
		//waterplane_drawable->hidden = false;
		


	}

	// WRITE TO REFLECT FRAME BUFFER
	{

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.reflect_fb);
		
		waterheight_direction.y = -1.0f;

		glUseProgram(lit_color_texture_program->program);
		glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f,1.0f)));
		glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform2fv(lit_color_texture_program->WATER_HEIGHT_DIRECTION_vec2, 1, glm::value_ptr(waterheight_direction));
		glUniform3fv(lit_color_texture_program->CAMPOS_vec3, 1, glm::value_ptr( camera->transform->make_local_to_world() * xyzvec));
		glUseProgram(0);

		//glm::vec3 reflectcam = camera->transform->make_local_to_world() * xyzvec;
		//float dist = 2.0f * abs(reflectcam.z - waterheight);
		
		//camera->transform->position.z += dist;
		//camera->transform->rotation.x = camera->transform->rotation.x * -1.0f;

		glClearColor(0.0f, 2.0f, 8.0f, 1.0f);
		glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CLIP_DISTANCE0);
		glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.
		scene.draw(*camera);

		//camera->transform->position.z -= dist;
		//camera->transform->rotation.x = camera->transform->rotation.x * -1.0f;

		waterplane_size->scale = scale;
	}
	
	{
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, framebuffers.reflect_tex);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, framebuffers.refract_tex);


		glUseProgram(wave_texture_program->program);
		glUniform1i(wave_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(wave_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
		glUniform3fv(wave_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform3fv(wave_texture_program->CAMPOS_vec3, 1, glm::value_ptr( camera->transform->make_local_to_world() * xyzvec));
		glUniform3fv(wave_texture_program->PLAYER_POS_vec3, 1, glm::value_ptr( puffer.get_position()));
		glUniform3fv(wave_texture_program->PLAYER_VEL_vec3, 1, glm::value_ptr( puffer.velocity ));
		glUniform1f(wave_texture_program->PLAYER_SCALE_float, puffer.current_scale);
		glUniform1f(wave_texture_program->TIME_float, elapsedtime);
		glUseProgram(0); 

		waterheight_direction.y = 0.0f;

		glUseProgram(lit_color_texture_program->program);
		glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f,1.0f)));
		glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform2fv(lit_color_texture_program->WATER_HEIGHT_DIRECTION_vec2, 1, glm::value_ptr(waterheight_direction));
		glUniform3fv(lit_color_texture_program->CAMPOS_vec3, 1, glm::value_ptr( camera->transform->make_local_to_world() * xyzvec));
		glUseProgram(0);

		//std::cout << (camera->transform->make_local_to_world() * xyzvec).z << std::endl;
		//std::cout << (waterheight_direction).x << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.0f, 2.0f, 8.0f, 1.0f);
		glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.
		scene.draw(*camera);
	}

	//draw the particles
	particle_system.draw();

	{
		//framebuffers.tone_map();
	}

	//example of setting up a button in the center of the screen, please remove when needed along with example_buttons field in playmode.hpp
	// for (Button& button : example_buttons)
	// 	button.draw(drawable_size);

	// ui_render_program->draw_ui(*font->get_text(std::string("this is a test, do not panic")), glm::vec2(0.5f),drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(1.0f), glm::vec3(0),true);
	// ui_render_program->draw_ui(*font->get_text(std::string("Hunger:")), glm::vec2(0.1f, .9f),drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(0.8f), glm::vec3(0),true);
	// ui_render_program->draw_ui(ui_elements.w, glm::vec2(0.5f),drawable_size);
	// ui_render_program->draw_ui(ui_elements.w_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Center, glm::vec2(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//draw hunger bar
	{
		if(Mode::current != menu && !is_game_over){
			float hunger_bar_scaling = 1.0f * (QTE::hunger / 100.0f);
			ui_render_program->draw_ui(ui_elements.hunger_bar_outline, glm::vec2(0.03f,0.05f),drawable_size,UIRenderProgram::BottomLeft,glm::vec2(0.7f,0.7f));
			ui_render_program->draw_ui(ui_elements.hunger_bar_fill, glm::vec2(0.0354f,0.06f),drawable_size,UIRenderProgram::BottomLeft,glm::vec2(0.7f,0.7f*hunger_bar_scaling));
			ui_render_program->draw_ui(ui_elements.hunger_bar_symbol, glm::vec2(0.015f,0.025f),drawable_size,UIRenderProgram::BottomLeft,glm::vec2(0.7f,0.7f));
		}
	}

	//draw oxygen bar
	{
		int num_bubbles = int(puffer.oxygen_level / 10.0f);
		if(puffer.above_water){
			{
				for(int index = 0; index < num_bubbles; index++){
					float bubble_vertical_position = 0.1f + (0.08f * index);
					ui_render_program->draw_ui(ui_elements.bubble, glm::vec2(0.95f,bubble_vertical_position),drawable_size, UIRenderProgram::BottomLeft,glm::vec2(0.13f,0.13f));
				}

			}
		}

	}

	//draw text for QTE trigger
	{
		glDisable(GL_DEPTH_TEST);
	
		if(Mode::current == shared_from_this()) {
			if(bait_in_eating_range && !qte_active){
				ui_render_program->draw_ui(*font->get_text(std::string("Press E to eat the bait")), glm::vec2(0.5f, 0.7f),drawable_size,UIRenderProgram::AlignMode::Center, glm::vec2(0.8f), glm::vec3(1),true);
			}
		}
	}

	GL_ERRORS();
}