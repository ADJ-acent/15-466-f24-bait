#include "Texture.hpp"
#include "Animation.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

UIElements ui_elements;

SpriteAnimation qte_timer_animation;

Load< void > load_textures(LoadTagDefault, []() -> void {
	stbi_set_flip_vertically_on_load(true);
	auto load_tex_to_GL = [&](std::string const &filename) {
        Texture texture;
		int channels;
		stbi_uc* data = stbi_load(filename.c_str(), &texture.width, &texture.height, &channels, 4); 
		if (data == nullptr) {
			std::runtime_error("Failed to load texture: "+ filename + ", " + stbi_failure_reason());
		}

		glGenTextures(1, &texture.handle);
		glBindTexture(GL_TEXTURE_2D, texture.handle);
		GL_ERRORS();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.width, texture.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		GL_ERRORS();
		glGenerateMipmap(GL_TEXTURE_2D); 

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);
		GL_ERRORS();
		return texture;
	};

    // declare textures above this function definition
    // then define them here:
    // health_UI_fill = load_tex_to_GL(data_path("ui/HamsterHealthFill.png"));
	ui_elements.w = load_tex_to_GL(data_path("ui/keyboard_w_outline.png"));
	ui_elements.w_pressed = load_tex_to_GL(data_path("ui/keyboard_w.png"));
	ui_elements.a = load_tex_to_GL(data_path("ui/keyboard_a_outline.png"));
	ui_elements.a_pressed = load_tex_to_GL(data_path("ui/keyboard_a.png"));
	ui_elements.s = load_tex_to_GL(data_path("ui/keyboard_s_outline.png"));
	ui_elements.s_pressed = load_tex_to_GL(data_path("ui/keyboard_s.png"));
	ui_elements.d = load_tex_to_GL(data_path("ui/keyboard_d_outline.png"));
	ui_elements.d_pressed = load_tex_to_GL(data_path("ui/keyboard_d.png"));
	ui_elements.hunger_bar_outline = load_tex_to_GL(data_path("ui/uihungerbar_empty.png"));
	ui_elements.hunger_bar_fill = load_tex_to_GL(data_path("ui/uihungerbar_bar.png"));
	ui_elements.oxygen_bar_fill = load_tex_to_GL(data_path("ui/oxygenbar0.png"));
	ui_elements.hunger_bar_symbol = load_tex_to_GL(data_path("ui/uihungerbar_symbol.png"));
	ui_elements.bubble = load_tex_to_GL(data_path("ui/bubble.png"));
	//load qte timer textures
	{
		uint32_t cur_index = 0;
		qte_timer_animation.frames.clear();
		while (true) {
			std::string cur_path = data_path("ui/qte_timer/qte_timer_" + std::to_string(cur_index)+".png");
			int x, y, channels;
			if (!stbi_info(cur_path.c_str(), &x, &y, &channels)) {
				break;
			}
			qte_timer_animation.frames.push_back(load_tex_to_GL(cur_path));
			cur_index++;
		}
		// std::cout<<"loaded "<<cur_index<< " frames of qte timer animation"<<std::endl;
	}
});