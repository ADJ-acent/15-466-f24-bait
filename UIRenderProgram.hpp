#pragma once

#include "GL.hpp"
#include "Load.hpp"
#include "Texture.hpp"
#include <glm/glm.hpp>

//Shader program that draws UIs on screen with ortho projection
struct UIRenderProgram {
	UIRenderProgram();
	~UIRenderProgram();

	GLuint program = 0;

	//Attribute (per-vertex variable) locations:
	GLuint PosTex_vec4 = -1U;


	//Uniform (per-invocation variable) locations:
	GLuint PROJECTION_mat4 = -1U;
	GLuint TexColor_vec3 = -1U;
	
	void draw_ui(Texture& texture, float scale, glm::vec2 position, glm::uvec2 drawable_size, glm::vec3 tint = glm::vec3(1.0f));
};

extern Load< UIRenderProgram > ui_render_program;
