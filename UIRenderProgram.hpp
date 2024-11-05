#pragma once

#include "GL.hpp"
#include "Load.hpp"
#include <glm/glm.hpp>

struct Texture;
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
	enum AlignMode : uint8_t{
		TopLeft,
		Top,
		TopRight,
		CenterLeft,
		Center,
		CenterRight,
		BottomLeft,
		Bottom,
		BottomRight,
	};
	
	void draw_ui(Texture& texture, glm::vec2 position, glm::uvec2 drawable_size, AlignMode align = BottomLeft, glm::vec2 scale = glm::vec2(1.0f), glm::vec3 tint = glm::vec3(1.0f)) const;
	// returns the relative offset to the width and height of the ui element
	static glm::vec2 get_align_offset(AlignMode align);
};

extern Load< UIRenderProgram > ui_render_program;
