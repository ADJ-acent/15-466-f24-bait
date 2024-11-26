#pragma once

#include "GL.hpp"
#include "Load.hpp"
#include <glm/glm.hpp>

struct Texture;
//Shader program that draws UIs on screen with ortho projection
struct ParticleProgram {
	ParticleProgram();
	~ParticleProgram();

	GLuint program = 0;

	GLuint VAO = 0;
	GLuint VBO = 0;

	//Attribute (per-vertex variable) locations:
	GLuint PosTex_vec4 = -1U;

	//Uniform (per-invocation variable) locations:
	GLuint PROJECTION_mat4 = -1U;
	GLuint TexColor_vec3 = -1U;
	void bind_particle(const uint32_t texture) const;
	void draw_particle(const glm::mat4& clip_from_local, const glm::vec3& tint) const;
	void unbind_particle() const;
};

extern Load< ParticleProgram > particle_program;
