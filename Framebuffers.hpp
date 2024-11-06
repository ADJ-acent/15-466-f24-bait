#pragma once

#include "GL.hpp"
#include <glm/glm.hpp>

//A global set of framebuffers for use in various offscreen rendering effects:

struct Framebuffers {
	void realloc(glm::uvec2 const &drawable_size); //called to trigger (re-)allocation on window size change

	//current size of framebuffer attachments:
	glm::uvec2 size = glm::uvec2(0,0);

	//for the "hdr tonemapping" rendering example:
	GLuint hdr_color_tex = 0; //GL_RGB16F color texture
	GLuint hdr_depth_rb = 0; //GL_DEPTH_COMPONENT24 renderbuffer
	GLuint hdr_fb = 0; // color0: color_tex , depth: depth_rb

	void tone_map(); //copy hdr.color_tex to screen with tone mapping applied

	//for the "bloom" effect:
	GLuint blur_x_tex = 0; //GL_RGB16F color texture for first pass of blur
	GLuint blur_x_fb = 0; // color0: blur_x_tex


	GLuint refract_tex = 0; //GL_RGB16F refract texture for water
	GLuint refract_depth_rb = 0;
	GLuint refract_fb = 0; // color0: refract_tex

	GLuint reflect_tex = 0; //GL_RGB16F reflect texture for water
	GLuint reflect_depth_rb = 0;
	GLuint reflect_fb = 0; // color0: reflect_tex


	void add_bloom(); //do a basic bloom effect on the hdr.color_tex
};

//the actual storage:
extern Framebuffers framebuffers;
//NOTE: I could have used a namespace and declared every element extern but that seemed more cumbersome to write
