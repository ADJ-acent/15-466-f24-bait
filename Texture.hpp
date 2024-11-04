#pragma once
#include "Load.hpp"
#include "gl_errors.hpp"

#include "stb_image.h"
#include "GL.hpp"
#include <iostream>

struct Texture {
    GLuint handle;
    int width;
    int height;
};

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
});