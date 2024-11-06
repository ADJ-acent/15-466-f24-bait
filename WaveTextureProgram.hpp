#pragma once

#include "GL.hpp"
#include "Load.hpp"
#include "Scene.hpp"


//Shader program that draws transformed, lit, textured vertices tinted with vertex colors:
struct WaveTextureProgram {
	WaveTextureProgram();
	~WaveTextureProgram();

	GLuint program = 0;

	//Attribute (per-vertex variable) locations:
	GLuint Position_vec4 = -1U;
	GLuint Normal_vec3 = -1U;
	GLuint Color_vec4 = -1U;
	GLuint TexCoord_vec2 = -1U;
	
	//Uniform (per-invocation variable) locations:
	GLuint OBJECT_TO_CLIP_mat4 = -1U;
	GLuint OBJECT_TO_LIGHT_mat4x3 = -1U;
	GLuint OBJECT_TO_WORLD_mat4 = -1U;
	GLuint NORMAL_TO_LIGHT_mat3 = -1U;

	//lighting:
	GLuint LIGHT_TYPE_int = -1U;
	GLuint LIGHT_LOCATION_vec3 = -1U;
	GLuint LIGHT_DIRECTION_vec3 = -1U;
	GLuint LIGHT_ENERGY_vec3 = -1U;
	GLuint LIGHT_CUTOFF_float = -1U;
	GLuint CAMPOS_vec3 = -1U;
	GLuint CAMROT_vec3 = -1U;
	GLuint TIME_float = -1U;


	//GLUint REFLECT_TEX_sampler2D = -1U;
	//GLUint REFRACT_TEX_sampler2D = -1U;

	
	
	//Textures:
	//TEXTURE0 - texture that is accessed by TexCoord
};

extern Load< WaveTextureProgram > wave_texture_program;

//For convenient scene-graph setup, copy this object:
// NOTE: by default, has texture bound to 1-pixel white texture -- so it's okay to use with vertex-color-only meshes.
extern Scene::Drawable::Pipeline wave_texture_program_pipeline;
