//Mode.hpp declares the "Mode::current" static member variable, which is used to decide where event-handling, updating, and drawing events go:
#include "Mode.hpp"

//The 'PlayMode' mode plays the game:
#include "PlayMode.hpp"
#include "MenuMode.hpp"

//For asset loading:
#include "Load.hpp"

//For sound init:
#include "Sound.hpp"

//GL.hpp will include a non-namespace-polluting set of opengl prototypes:
#include "GL.hpp"

//for screenshots:
#include "load_save_png.hpp"

//Includes for libSDL:
#include <SDL.h>

// image loading
#include "stb_image.h"

//...and for c++ standard library functions:
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <memory>
#include <algorithm>

std::shared_ptr< PlayMode > play;
std::shared_ptr< MenuMode > menu;

#ifdef _WIN32
extern "C" { uint32_t GetACP(); }
#endif
int main(int argc, char **argv) {
#ifdef _WIN32
	{ //when compiled on windows, check that code page is forced to utf-8 (makes file loading/saving work right):
		//see: https://docs.microsoft.com/en-us/windows/apps/design/globalizing/use-utf8-code-page
		uint32_t code_page = GetACP();
		if (code_page == 65001) {
			std::cout << "Code page is properly set to UTF-8." << std::endl;
		} else {
			std::cout << "WARNING: code page is set to " << code_page << " instead of 65001 (UTF-8). Some file handling functions may fail." << std::endl;
		}
	}

	//when compiled on windows, unhandled exceptions don't have their message printed, which can make debugging simple issues difficult.
	try {
#endif

	//------------  initialization ------------

	//Initialize SDL library:
	SDL_Init(SDL_INIT_VIDEO);

	//Ask for an OpenGL context version 3.3, core profile, enable debug:
	SDL_GL_ResetAttributes();
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	//create window:
	SDL_Window *window = SDL_CreateWindow(
		"BAIT: PUFFER FISH SIMULATOR", //TODO: remember to set a title for your game!
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		1280, 720, //TODO: modify window size if you'd like
		SDL_WINDOW_OPENGL
		| SDL_WINDOW_RESIZABLE //uncomment to allow resizing
		| SDL_WINDOW_ALLOW_HIGHDPI //uncomment for full resolution on high-DPI screens
	);

	//prevent exceedingly tiny windows when resizing:
	SDL_SetWindowMinimumSize(window,100,100);

	if (!window) {
		std::cerr << "Error creating SDL window: " << SDL_GetError() << std::endl;
		return 1;
	}

	//Create OpenGL context:
	SDL_GLContext context = SDL_GL_CreateContext(window);

	if (!context) {
		SDL_DestroyWindow(window);
		std::cerr << "Error creating OpenGL context: " << SDL_GetError() << std::endl;
		return 1;
	}

	//On windows, load OpenGL entrypoints: (does nothing on other platforms)
	init_GL();

	//Set VSYNC + Late Swap (prevents crazy FPS):
	if (SDL_GL_SetSwapInterval(-1) != 0) {
		std::cerr << "NOTE: couldn't set vsync + late swap tearing (" << SDL_GetError() << ")." << std::endl;
		if (SDL_GL_SetSwapInterval(1) != 0) {
			std::cerr << "NOTE: couldn't set vsync (" << SDL_GetError() << ")." << std::endl;
		}
	}

	//Set automatic SRGB encoding if framebuffer needs it:
	glEnable(GL_FRAMEBUFFER_SRGB);


	Uint8 * cursor_image;
	{// set up custom mouse curser
		//upload the cursor
		int cursor_width, cursor_height, channels;
		cursor_image = stbi_load(data_path("ui/cursor.png").c_str(), &cursor_width, &cursor_height, &channels, 0);
		Sint32 dot_pitch = cursor_width * channels;
		dot_pitch = (dot_pitch + 3) & ~3;

		Sint32 red_mask;
		Sint32 green_mask;
		Sint32 blue_mask;
		Sint32 alpha_mask;

		// This only works with little endian computer processors,
		// if you want support for big endian (if you even still use it), implement it
		// yourself.
		red_mask = 0x000000FF;
		green_mask = 0x0000FF00;
		blue_mask = 0x00FF0000;
		alpha_mask = (channels == 4) ? 0xFF000000 : 0;

		SDL_Surface *cursor_surface = SDL_CreateRGBSurfaceFrom(
			cursor_image, cursor_width, cursor_height, channels * 8, dot_pitch, red_mask, green_mask,
			blue_mask, alpha_mask);
		if (!cursor_surface) {
			stbi_image_free(cursor_image);
			cursor_image = NULL;
			std::cerr<< "Error creating custom cursor, using normal cursor instead\n";
		}
		else {
			SDL_Cursor* cursor = SDL_CreateColorCursor(cursor_surface, 4,1);
			SDL_SetCursor(cursor);
		}
	}


	//------------ init sound --------------
	Sound::init();

	//------------ load assets --------------
	call_load_functions();

	//------------ create game mode + make current --------------
	play = std::make_shared< PlayMode >();
	menu = std::make_shared< MenuMode >();
	
	menu->background = play;

	// menu->start_choices.emplace_back("BAIT");
	
	// menu->start_choices.emplace_back("Play", [&](){
	// 	menu->is_before_game_start = false;
	// 	Mode::set_current(play);
	// }, true);
	// menu->start_menu_buttons.push_back(menu->start_choices.back().button);

	// menu->start_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
	// menu->start_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

	// menu->start_choices.emplace_back("Exit", [&](){
	// 	Mode::set_current(nullptr);
	// }, true);
	// menu->start_menu_buttons.push_back(menu->start_choices.back().button);

	// menu->start_menu_buttons.back().set_hover_state(glm::vec2(1.05f), glm::vec3(0.05f));
	// menu->start_menu_buttons.back().set_pressing_state(glm::vec2(0.95f), glm::vec3(0.5f, 0.0f, 0.0f));

	// menu->selected = 1;


	Mode::set_current(menu);

	// Mode::set_current(std::make_shared< PlayMode >());

	//------------ main loop ------------

	//this inline function will be called whenever the window is resized,
	// and will update the window_size and drawable_size variables:
	glm::uvec2 window_size; //size of window (layout pixels)
	glm::uvec2 drawable_size; //size of drawable (physical pixels)
	//On non-highDPI displays, window_size will always equal drawable_size.
	auto on_resize = [&](){
		int w,h;
		SDL_GetWindowSize(window, &w, &h);
		window_size = glm::uvec2(w, h);
		SDL_GL_GetDrawableSize(window, &w, &h);
		drawable_size = glm::uvec2(w, h);
		glViewport(0, 0, drawable_size.x, drawable_size.y);
	};
	on_resize();

	//This will loop until the current mode is set to null:
	while (Mode::current) {
		//every pass through the game loop creates one frame of output
		//  by performing three steps:

		{ //(1) process any events that are pending
			static SDL_Event evt;
			while (SDL_PollEvent(&evt) == 1) {
				//handle resizing:
				if (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					on_resize();
				}
				//handle input:
				if (Mode::current && Mode::current->handle_event(evt, window_size)) {
					// mode handled it; great
				} else if (evt.type == SDL_QUIT) {
					Mode::set_current(nullptr);
					break;
				} else if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_PRINTSCREEN) {
					// --- screenshot key ---
					std::string filename = "screenshot.png";
					std::cout << "Saving screenshot to '" << filename << "'." << std::endl;
					glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
					glReadBuffer(GL_FRONT);
					int w,h;
					SDL_GL_GetDrawableSize(window, &w, &h);
					std::vector< glm::u8vec4 > data(w*h);
					glReadPixels(0,0,w,h, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
					for (auto &px : data) {
						px.a = 0xff;
					}
					save_png(filename, glm::uvec2(w,h), data.data(), LowerLeftOrigin);
				}
			}
			if (!Mode::current) break;
		}

		{ //(2) call the current mode's "update" function to deal with elapsed time:
			auto current_time = std::chrono::high_resolution_clock::now();
			static auto previous_time = current_time;
			float elapsed = std::chrono::duration< float >(current_time - previous_time).count();
			previous_time = current_time;

			//if frames are taking a very long time to process,
			//lag to avoid spiral of death:
			elapsed = std::min(0.1f, elapsed);

			Mode::current->update(elapsed);
			if (!Mode::current) break;
		}

		{ //(3) call the current mode's "draw" function to produce output:
		
			Mode::current->draw(drawable_size);
		}

		//Wait until the recently-drawn frame is shown before doing it all again:
		SDL_GL_SwapWindow(window);
	}


	//------------  teardown ------------
	if (cursor_image != NULL) {
		stbi_image_free(cursor_image);
	}
	Sound::shutdown();

	SDL_GL_DeleteContext(context);
	context = 0;

	SDL_DestroyWindow(window);
	window = NULL;

	return 0;

#ifdef _WIN32
	} catch (std::exception const &e) {
		std::cerr << "Unhandled exception:\n" << e.what() << std::endl;
		return 1;
	} catch (...) {
		std::cerr << "Unhandled exception (unknown type)." << std::endl;
		throw;
	}
#endif
}
