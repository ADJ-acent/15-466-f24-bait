#include "PlayMode.hpp"

#include "DepthTextureProgram.hpp"
#include "LitColorTextureProgram.hpp"
#include "WaveTextureProgram.hpp"
#include "WiggleTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Framebuffers.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint main_scene_for_depth_texture_program = 0;
GLuint puffer_scene_for_depth_texture_program = 0;
GLuint bait_scene_for_depth_texture_program = 0;
GLuint waterplane_scene_for_wave_texture_program = 0;
GLuint seaweed_objs_for_wiggle_texture_program = 0;
Load< MeshBuffer > main_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/ocean_scene.pnct"));
	main_scene_for_depth_texture_program = ret->make_vao_for_program(depth_texture_program->program);
	seaweed_objs_for_wiggle_texture_program = ret->make_vao_for_program(wiggle_texture_program->program);
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

Load< MeshBuffer > waterplane_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/waterplane_scene.pnct"));
	waterplane_scene_for_wave_texture_program = ret->make_vao_for_program(wave_texture_program->program);
	return ret;
});


Load< Scene > main_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/ocean_scene.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = main_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		if(mesh_name.find("seaweed") == -1)
		{
			if(mesh_name.find("sand") == -1)
			{
				drawable.pipeline = lit_color_texture_program_pipeline;
				drawable.pipeline.vao = main_scene_for_depth_texture_program;
			}
			else
			{
				drawable.pipeline = depth_texture_program_pipeline;
				drawable.pipeline.vao = main_scene_for_depth_texture_program;
			}
			
		}
		else
		{
			drawable.pipeline = wiggle_texture_program_pipeline;
			drawable.pipeline.vao = seaweed_objs_for_wiggle_texture_program;
		}

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

		drawable.pipeline.vao = puffer_scene_for_depth_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

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

	});
});

Load< Scene > waterplane_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/waterplane_scene.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = waterplane_meshes->lookup(mesh_name);

		scene.drawables.emplace_back(transform);
		Scene::Drawable &drawable = scene.drawables.back();

		drawable.pipeline = wave_texture_program_pipeline;

		drawable.pipeline.vao = waterplane_scene_for_wave_texture_program;
		drawable.pipeline.type = mesh.type;
		drawable.pipeline.start = mesh.start;
		drawable.pipeline.count = mesh.count;

	});
});

PlayMode::PlayMode() : scene(*main_scene) {

	scene.add(*waterplane_scene);

	std::vector<Scene::Transform *> puffer_transforms = scene.spawn(*puffer_scene,PUFFER);
	puffer.init(puffer_transforms);

	eat_bait_QTE = new QTE();

	

	Bait bait_1 = Bait();
	std::vector<Scene::Transform *> bait_1_transforms = scene.spawn(*bait_scene,CARROT_BAIT);
	bait_1.init(bait_1_transforms,0);

	QTE::active_baits.emplace_back(bait_1);

	fish_collider = calculate_collider(puffer.main_transform, pufferfish_meshes->lookup("PuffBody"));

	for(Bait b : QTE::active_baits){
    	
		if(b.type_of_bait==0){
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
			//TODO, disable eating pressed being registered when qte is active
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
	
	elapsedtime += elapsed;

	int8_t swim_direction = int8_t(right.pressed) - int8_t(left.pressed);
	puffer.update(mouse_motion, swim_direction, elapsed);

	
	//collision check:
	//if fish collided with bait and eat button pressed
	//new QTE initialized
	{		
		
		glm::vec3 puffer_position = puffer.get_position();
		glm::vec3 puffer_view = puffer.get_forward();

		Bait* best_bait = nullptr;
		float closest_in_view_bait = 1000.0f;
		bait_in_eating_range = false;
		for(Bait b : QTE::active_baits){
			glm::vec3 bait_position = b.get_position();
			glm::vec3 puff_to_bait = bait_position - puffer_position;
			float distance_squared = glm::dot(puff_to_bait, puff_to_bait);

			float cosine_angle = glm::dot(puffer_view, puff_to_bait) / glm::length(puffer_view) * glm::length(puff_to_bait);

			// bait must be in front of camera
			bool bait_in_view = puff_to_bait == glm::vec3(0) || cosine_angle > 0.707f;//0.707f is cos45, probably should store inside bait

			if(distance_squared <= Bait::eat_distance_threshold_squared && bait_in_view &&
				!qte_active){
				if (eat.pressed && distance_squared < closest_in_view_bait) {
					best_bait = &b;
					closest_in_view_bait = distance_squared;
				}
				else {
					bait_in_eating_range = true;
				}
			}
		}

		if (best_bait != nullptr) {
			qte_active = true;

			eat_bait_QTE = new QTE(&puffer,best_bait-> mesh_parts.bait_string,best_bait-> mesh_parts.bait_base);
			if(best_bait->type_of_bait==0){
				eat_bait_QTE->start(3); //circle
			} else {
				eat_bait_QTE->start(5); //square
			}
		}
	}

	{
		eat_bait_QTE->update(elapsed);

		if(!eat_bait_QTE->active){
			qte_active = false;
		}
		
		//respawn a new bait here
		if(eat_bait_QTE->respawn_new_bait == true){
			Bait new_bait = Bait();
			//pick either square or circle
			std::srand(static_cast<unsigned int>(std::time(nullptr)));
			auto circle_or_square = rand() % 2; // 0 or 1
			std::vector<Scene::Transform *> new_bait_transforms;
			if(circle_or_square==0){
				new_bait_transforms = scene.spawn(*bait_scene, CARROT_BAIT);
			} else {
				new_bait_transforms = scene.spawn(*bait_scene, FISH_BAIT);
			}
			new_bait.init(new_bait_transforms, circle_or_square);
			QTE::active_baits.pop_back();
			QTE::active_baits.push_back(new_bait);
			new_bait.random_respawn_location();
			eat_bait_QTE->respawn_new_bait = false;
		}
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
	//framebuffers.realloc(drawable_size);
	camera->aspect = float(drawable_size.x) / float(drawable_size.y);

	//set up light type and position for depth_texture_program:
	// all the shaders
	{
		glUseProgram(depth_texture_program->program);
		glUniform1i(depth_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(depth_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
		glUniform3fv(depth_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform1f(depth_texture_program->TIME_float, elapsedtime);
		glUseProgram(0);


		glUseProgram(lit_color_texture_program->program);
		glUniform1i(lit_color_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(lit_color_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f,1.0f)));
		glUniform3fv(lit_color_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUseProgram(0);

		glUseProgram(wave_texture_program->program);
		glUniform1i(wave_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(wave_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
		glUniform3fv(wave_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform1f(wave_texture_program->TIME_float, elapsedtime);
		glUseProgram(0);


		glUseProgram(wiggle_texture_program->program);
		glUniform1i(wiggle_texture_program->LIGHT_TYPE_int, 1);
		glUniform3fv(wiggle_texture_program->LIGHT_DIRECTION_vec3, 1, glm::value_ptr(glm::vec3(0.0f, 0.0f,-1.0f)));
		glUniform3fv(wiggle_texture_program->LIGHT_ENERGY_vec3, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.95f)));
		glUniform1f(wiggle_texture_program->TIME_float, elapsedtime);
		glUniform3fv(wiggle_texture_program->PLAYERPOS_vec3, 1, glm::value_ptr( puffer.get_position()));
		glUseProgram(0);
	}

	
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffers.hdr_fb);

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClearDepth(1.0f); //1.0 is actually the default value to clear the depth buffer to, but FYI you can change it.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); //this is the default depth comparison function, but FYI you can change it.

	scene.draw(*camera);

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//framebuffers.add_oceandepth();
	//framebuffers.tone_map();

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

		if(eat_bait_QTE->active && eat_bait_QTE->input_delay <= 0 && !eat_bait_QTE->failure){
			lines.draw_text(eat_bait_QTE->get_prompt(),
				glm::vec3(-aspect + 2.0f * H, -1.0 + 2.0f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text(eat_bait_QTE->get_prompt(),
				glm::vec3(-aspect + 2.0f * H + ofs, -1.0 + + 2.0f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0xff * (eat_bait_QTE->red_text_percentage / eat_bait_QTE->time_limit), 
							0x00, 
							0x00, 0x00));
		}
		else if(bait_in_eating_range && !qte_active && !eat_bait_QTE->failure){
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
		else if(eat_bait_QTE->failure){
			lines.draw_text("Baited!!! FINAL SCORE: " + std::to_string(QTE::score),
				glm::vec3(-aspect + 2.0f * H, -1.0 + 2.0f * H, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
			float ofs = 2.0f / drawable_size.y;
			lines.draw_text("Baited!!! FINAL SCORE: " + std::to_string(QTE::score),
				glm::vec3(-aspect + 2.0f * H + ofs, -1.0 + + 2.0f * H + ofs, 0.0),
				glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
				glm::u8vec4(0x00, 0x00, 0x00, 0x00));
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