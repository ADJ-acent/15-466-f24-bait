#pragma once
#include "Load.hpp"
#include "gl_errors.hpp"
#include "data_path.hpp"
#include "GL.hpp"
#include <iostream>

struct Texture {
    GLuint handle;
    int width;
    int height;
};

struct UIElements {
	Texture w;
	Texture w_pressed;
	Texture a;
	Texture a_pressed;
	Texture s;
	Texture s_pressed;
	Texture d;
	Texture d_pressed;
	Texture hunger_bar_outline;
	Texture hunger_bar_fill;
	Texture oxygen_bar_fill;
	Texture hunger_bar_symbol;
	Texture bubble;
	Texture end_screen;
	Texture instructions;
	Texture logo;
};

struct ParticleTextures {
	Texture bubbles[7];
};