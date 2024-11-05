#include "WiggleTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Scene::Drawable::Pipeline wiggle_texture_program_pipeline;

Load< WiggleTextureProgram > wiggle_texture_program(LoadTagEarly, []() -> WiggleTextureProgram const * {
	WiggleTextureProgram *ret = new WiggleTextureProgram();

	//----- build the pipeline template -----
	wiggle_texture_program_pipeline.program = ret->program;

	wiggle_texture_program_pipeline.OBJECT_TO_CLIP_mat4 = ret->OBJECT_TO_CLIP_mat4;
    wiggle_texture_program_pipeline.OBJECT_TO_WORLD_mat4 = ret->OBJECT_TO_WORLD_mat4;
    wiggle_texture_program_pipeline.WORLD_TO_CLIP_mat4 = ret-> WORLD_TO_CLIP_mat4;
	wiggle_texture_program_pipeline.OBJECT_TO_LIGHT_mat4x3 = ret->OBJECT_TO_LIGHT_mat4x3;
	wiggle_texture_program_pipeline.NORMAL_TO_LIGHT_mat3 = ret->NORMAL_TO_LIGHT_mat3;

	/* This will be used later if/when we build a light loop into the Scene:
	wiggle_texture_program_pipeline.LIGHT_TYPE_int = ret->LIGHT_TYPE_int;
	wiggle_texture_program_pipeline.LIGHT_LOCATION_vec3 = ret->LIGHT_LOCATION_vec3;
	wiggle_texture_program_pipeline.LIGHT_DIRECTION_vec3 = ret->LIGHT_DIRECTION_vec3;
	wiggle_texture_program_pipeline.LIGHT_ENERGY_vec3 = ret->LIGHT_ENERGY_vec3;
	wiggle_texture_program_pipeline.LIGHT_CUTOFF_float = ret->LIGHT_CUTOFF_float;
	*/

	//make a 1-pixel white texture to bind by default:
	GLuint tex;
	glGenTextures(1, &tex);

	glBindTexture(GL_TEXTURE_2D, tex);
	std::vector< glm::u8vec4 > tex_data(1, glm::u8vec4(0xff));
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);


	wiggle_texture_program_pipeline.textures[0].texture = tex;
	wiggle_texture_program_pipeline.textures[0].target = GL_TEXTURE_2D;

	return ret;
});

WiggleTextureProgram::WiggleTextureProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
        "uniform mat4 OBJECT_TO_WORLD;\n"
        "uniform mat4 WORLD_TO_CLIP;\n"
		"uniform mat4x3 OBJECT_TO_LIGHT;\n"
		"uniform mat3 NORMAL_TO_LIGHT;\n"
        "uniform float TIME;\n"
        "uniform vec3 PLAYERPOS;\n"
        "uniform float PLAYERSCALE;\n"
		"in vec4 Position;\n"
		"in vec3 Normal;\n"
		"in vec4 Color;\n"
		"in vec2 TexCoord;\n"
		"out vec3 position;\n"
		"out vec3 normal;\n"
		"out vec4 color;\n"
		"out vec2 texCoord;\n"
        "out mat4 objclip;\n"
		"out vec4 postrans;\n"

        "	vec2 noise2x2(vec2 p) {\n"
		"	float x = dot(p, vec2(123.4, 234.5));\n"
		"	float y = dot(p, vec2(345.6, 456.7));\n"
		"	vec2 noise = vec2(x, y);\n"
		"	noise = sin(noise);\n"
		"	noise = noise * 43758.5453;\n"
		"	noise = fract(noise);\n"
		"	return noise;\n"
		"	}\n"

		"void main() {\n"
		
        "   vec3 worldpos = (OBJECT_TO_WORLD * Position).xyz;\n"
        "   vec3 offset = (worldpos - PLAYERPOS);\n"
        "   float distplayer = length(offset);\n"
        "   worldpos.z += (((min(distplayer,20.0) - 20.0) * TexCoord.x) * PLAYERSCALE) * 0.5;\n"
        "   offset = (worldpos - PLAYERPOS);\n"
        "   distplayer = length(offset);\n"
        "   offset = normalize(offset);\n"

		//"	gl_Position = OBJECT_TO_CLIP * Position;\n"
        "	float oceanwiggle =  sin(TIME + TexCoord.x * 1.5 + worldpos.x/10.0 + worldpos.y/10.0) * TexCoord.x ;\n"
        "   worldpos.x += ( oceanwiggle * 2.0 + offset.x * 2.0 * TexCoord.x);\n"
        "   worldpos.y += ( oceanwiggle * 2.0 + offset.y * 2.0 * TexCoord.x);\n"
        
        "   gl_Position = WORLD_TO_CLIP * vec4(worldpos,1.0);\n"
		"	position = OBJECT_TO_LIGHT * Position;\n"
		"	normal = NORMAL_TO_LIGHT * Normal;\n"
		"	color = Color;\n"
        "	texCoord = TexCoord;\n"
        "	objclip = OBJECT_TO_CLIP;\n"
		"	postrans = Position;\n"
		"}\n"
	,
		//fragment shader:
		"#version 330\n"
		"uniform sampler2D TEX;\n"
		"uniform int LIGHT_TYPE;\n"
		"uniform vec3 LIGHT_LOCATION;\n"
		"uniform vec3 LIGHT_DIRECTION;\n"
		"uniform vec3 LIGHT_ENERGY;\n"
		"uniform float LIGHT_CUTOFF;\n"
		"uniform float HIGHLIGHT;\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec4 color;\n"
		"in mat4 objclip;\n"
		"in vec4 postrans;\n"
		"in vec2 texCoord;\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"	vec3 n = normalize(normal);\n"
		"   float threshold = 150.0f;\n"
		"	vec3 oceanshade = vec3(0.2,0.6, 0.7);\n"
		"	float fog = min(((objclip * postrans).z/threshold),1.0);\n"
		"	vec4 albedo = texture(TEX, texCoord) * color;\n"
		"	vec3 base = mix( albedo.xyz , oceanshade,fog);\n"
		"	fragColor = vec4( base, 1.0);\n"
		"}\n"
	);
	//As you can see above, adjacent strings in C/C++ are concatenated.
	// this is very useful for writing long shader programs inline.

	//look up the locations of vertex attributes:
	Position_vec4 = glGetAttribLocation(program, "Position");
	Normal_vec3 = glGetAttribLocation(program, "Normal");
	Color_vec4 = glGetAttribLocation(program, "Color");
	TexCoord_vec2 = glGetAttribLocation(program, "TexCoord");
	

	//look up the locations of uniforms:
	OBJECT_TO_CLIP_mat4 = glGetUniformLocation(program, "OBJECT_TO_CLIP");
    OBJECT_TO_WORLD_mat4 = glGetUniformLocation(program, "OBJECT_TO_WORLD");
    WORLD_TO_CLIP_mat4 = glGetUniformLocation(program, "WORLD_TO_CLIP");
	OBJECT_TO_LIGHT_mat4x3 = glGetUniformLocation(program, "OBJECT_TO_LIGHT");
	NORMAL_TO_LIGHT_mat3 = glGetUniformLocation(program, "NORMAL_TO_LIGHT");

	LIGHT_TYPE_int = glGetUniformLocation(program, "LIGHT_TYPE");
	LIGHT_LOCATION_vec3 = glGetUniformLocation(program, "LIGHT_LOCATION");
	LIGHT_DIRECTION_vec3 = glGetUniformLocation(program, "LIGHT_DIRECTION");
	LIGHT_ENERGY_vec3 = glGetUniformLocation(program, "LIGHT_ENERGY");
	LIGHT_CUTOFF_float = glGetUniformLocation(program, "LIGHT_CUTOFF");
	TIME_float =  glGetUniformLocation(program, "TIME");
    PLAYERPOS_vec3 =  glGetUniformLocation(program, "PLAYERPOS");
    PLAYERSCALE_float =  glGetUniformLocation(program, "PLAYERSCALE");


	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now


}

WiggleTextureProgram::~WiggleTextureProgram() {
	glDeleteProgram(program);
	program = 0;
}

