#include "PlayMode.hpp"

#include "DepthTextureProgram.hpp"
#include "LitColorTextureProgram.hpp"
#include "WaveTextureProgram.hpp"
#include "WiggleTextureProgram.hpp"

#include "DrawLines.hpp"
#include "Mesh.hpp"
#include "Load.hpp"
#include "Texture.hpp"
#include "UIRenderProgram.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "Framebuffers.hpp"
#include "GameConfig.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <random>

GLuint main_scene_for_depth_texture_program = 0;
GLuint puffer_scene_for_depth_texture_program = 0;
GLuint bait_scene_for_depth_texture_program = 0;
GLuint waterplane_scene_for_wave_texture_program = 0;
GLuint seaweed_objs_for_wiggle_texture_program = 0;

Scene::Drawable *waterplane_drawable = nullptr;

Load< MeshBuffer > main_meshes(LoadTagDefault, []() -> MeshBuffer const * {
	MeshBuffer const *ret = new MeshBuffer(data_path("meshes/ocean_scene.pnct"));
	main_scene_for_depth_texture_program = ret->make_vao_for_program(depth_texture_program->program);
	seaweed_objs_for_wiggle_texture_program = ret->make_vao_for_program(wiggle_texture_program->program);
	waterplane_scene_for_wave_texture_program = ret->make_vao_for_program(wave_texture_program->program);
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


Load< Scene > main_scene(LoadTagDefault, []() -> Scene const * {
	return new Scene(data_path("scenes/ocean_scene.scene"), [&](Scene &scene, Scene::Transform *transform, std::string const &mesh_name){
		Mesh const &mesh = main_meshes->lookup(mesh_name);

		

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
		else if(mesh_name.find("waterplane") != -1)
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


extern UIElements ui_elements;
extern Load< UIRenderProgram > ui_render_program;
GameConfig game_config;

PlayMode::PlayMode() : scene(*main_scene) {


	std::vector<Scene::Transform *> puffer_transforms = scene.spawn(*puffer_scene,PUFFER);

	puffer.init(puffer_transforms, &scene);


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

	for(Bait b : bait_manager.baits_in_use){
    	
		if(b.type_of_bait == 0){
			b.string_collider = calculate_collider(b.mesh_parts.bait_string, bait_meshes->lookup("carrotbait_string"));
			b.bait_collider = calculate_collider(b.mesh_parts.bait_base, bait_meshes->lookup("carrotbait_base"));
		} else {
			b.string_collider = calculate_collider(b.mesh_parts.bait_string, bait_meshes->lookup("fishbait_string"));
			b.bait_collider = calculate_collider(b.mesh_parts.bait_base, bait_meshes->lookup("fishbait_base"));
		}

	}

	//get pointer to camera for convenience:
	for (auto& cam : scene.cameras) {
		if (cam.transform->name == "PuffCam") {
			camera = &cam;
		}
	}


	for (auto& transform : scene.transforms) {
		if (transform.name.find("waterplane") != -1) {
			Scene::Transform*temp = &transform;
			waterplane_size = temp;
			waterheight = temp->position.z;

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
	if (debug.downs != 0) {
		game_config.charge_face_camera = !game_config.charge_face_camera;
	}
	
	elapsedtime += elapsed;

	int8_t swim_direction = int8_t(right.pressed) - int8_t(left.pressed);
	puffer.update(mouse_motion, swim_direction, elapsed);

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

	score_decrement_counter += elapsed;
    if(score_decrement_counter>5.0f){
        score_decrement_counter = 0.0f;
        QTE::score -= 1;
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
		glUniform3fv(wave_texture_program->CAMROT_vec3, 1, glm::value_ptr( camera->transform->rotation));
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


	{





		//framebuffers.tone_map();

	}
	

	// ui_render_program->draw_ui(ui_elements.w, glm::vec2(0.5f),drawable_size);
	// ui_render_program->draw_ui(ui_elements.w_pressed, glm::vec2(0.5f), drawable_size, UIRenderProgram::AlignMode::Center, glm::vec2(3.0f), glm::vec3(1.0f, 0.0f, 0.0f));


	{ //use DrawLines to overlay some text:
		glDisable(GL_DEPTH_TEST);
		float aspect = float(drawable_size.x) / float(drawable_size.y);
		DrawLines lines(glm::mat4(
			1.0f / aspect, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		));

		DrawLines lines_mesh(camera->make_projection() * glm::mat4(camera->transform->make_world_to_local()));

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

		lines.draw_text("Hunger: " + std::to_string(QTE::score),
			glm::vec3(-aspect + 0.1f * H, -1.0 + 5.0f * H, 0.0),
			glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
			glm::u8vec4(0x00, 0x00, 0x00, 0x00));
		float ofs = 2.0f / drawable_size.y;
        lines.draw_text("Hunger: " + std::to_string(QTE::score),
            glm::vec3(-aspect + 0.1f * H + ofs, -1.0 + + 5.0f * H + ofs, 0.0),
            glm::vec3(H, 0.0f, 0.0f), glm::vec3(0.0f, H, 0.0f),
            glm::u8vec4(0xff, 0xff, 0xff, 0x00));
	}

	GL_ERRORS();
}