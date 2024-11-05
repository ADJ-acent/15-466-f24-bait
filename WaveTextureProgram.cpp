#include "WaveTextureProgram.hpp"

#include "gl_compile_program.hpp"
#include "gl_errors.hpp"

Scene::Drawable::Pipeline wave_texture_program_pipeline;

Load< WaveTextureProgram > wave_texture_program(LoadTagEarly, []() -> WaveTextureProgram const * {
	WaveTextureProgram *ret = new WaveTextureProgram();

	//----- build the pipeline template -----
	wave_texture_program_pipeline.program = ret->program;

	wave_texture_program_pipeline.OBJECT_TO_CLIP_mat4 = ret->OBJECT_TO_CLIP_mat4;
	wave_texture_program_pipeline.OBJECT_TO_LIGHT_mat4x3 = ret->OBJECT_TO_LIGHT_mat4x3;
	wave_texture_program_pipeline.OBJECT_TO_WORLD_mat4 = ret->OBJECT_TO_WORLD_mat4;
	wave_texture_program_pipeline.NORMAL_TO_LIGHT_mat3 = ret->NORMAL_TO_LIGHT_mat3;

	/* This will be used later if/when we build a light loop into the Scene:
	wave_texture_program_pipeline.LIGHT_TYPE_int = ret->LIGHT_TYPE_int;
	wave_texture_program_pipeline.LIGHT_LOCATION_vec3 = ret->LIGHT_LOCATION_vec3;
	wave_texture_program_pipeline.LIGHT_DIRECTION_vec3 = ret->LIGHT_DIRECTION_vec3;
	wave_texture_program_pipeline.LIGHT_ENERGY_vec3 = ret->LIGHT_ENERGY_vec3;
	wave_texture_program_pipeline.LIGHT_CUTOFF_float = ret->LIGHT_CUTOFF_float;
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


	wave_texture_program_pipeline.textures[0].texture = tex;
	wave_texture_program_pipeline.textures[0].target = GL_TEXTURE_2D;

	return ret;
});

WaveTextureProgram::WaveTextureProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330\n"
		"uniform mat4 OBJECT_TO_CLIP;\n"
		"uniform mat4x3 OBJECT_TO_LIGHT;\n"
		"uniform mat4 OBJECT_TO_WORLD;\n"
		"uniform mat3 NORMAL_TO_LIGHT;\n"
        "uniform float TIME;\n"
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

        //noise function 
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

        "	vec2 uv = TexCoord * 24.0;\n"
		"	vec2 currgridid = floor(uv);\n"
		"	vec2 currentgridcoord = fract(uv);\n"
		"	currentgridcoord = currentgridcoord - 0.5;\n"
		" 	vec2 redgriduv = currentgridcoord;\n"
		"	redgriduv = abs(redgriduv);\n"
		"	float distedge = 2.0 * max(redgriduv.x, redgriduv.y);\n"
		
		"	float pointsongrid = 0.0;\n"
		"	float mindist = 100.0;\n"
		
		//"	vec3 redgridcolor = vec3(smoothstep(0.9,1.0,distedge),0.0,0.0);\n"//SHOWS THE GRIDLINE

		"	for(float i = -1.0; i <= 1.0; i++){\n"
		"		for(float j = -1.0; j <= 1.0; j++){\n"
		"			vec2 adjgridcoords = vec2(i, j);\n"
		"			vec2 pointonadjgrid = adjgridcoords;\n"
		
		"			vec2 noise = noise2x2(currgridid + adjgridcoords);\n"
    
		"			pointonadjgrid = adjgridcoords + sin(TIME * noise)* 0.5;\n"
		"			float dist = length(currentgridcoord - pointonadjgrid);\n"
		"			mindist = min(dist, mindist);\n"
		"			pointsongrid += smoothstep(0.95, 0.96, 1.0-dist);\n"
		"	}\n"
		"}\n"
		
		//"	vec3 pointsongridcolor = vec3(pointsongrid);\n" //SHOWS THE POINTS ON THE GRID
		
		"	float oceanwave =  smoothstep(0.0,1.0,mindist);\n"

		"	gl_Position = OBJECT_TO_CLIP * Position;\n"
        "   gl_Position.y += 5.0 * oceanwave;\n"
		"	position = (OBJECT_TO_WORLD * Position).xyz;\n"
		"	normal = Normal;\n"
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
		"uniform vec3 CAMPOS;\n"
		"uniform vec3 CAMROT;\n"
		"uniform vec3 LIGHT_ENERGY;\n"
		"uniform float LIGHT_CUTOFF;\n"
		"uniform float TIME;\n"
		"in vec3 position;\n"
		"in vec3 normal;\n"
		"in vec4 color;\n"
		"in vec2 texCoord;\n"
		"in mat4 objclip;\n"
		"in vec4 postrans;\n"
		"in float time;\n"
		"out vec4 fragColor;\n"

		//noise function 
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
        "   float threshold = 200.0f;\n" //the viewing threshold for the fog
		"	vec3 n = normalize(normal);\n" //normalized vector of the normal from the fragment
		"	vec3 oceanshade = vec3(0.2,0.6, 0.7);\n" //shade of the ocean being used

		"	vec3 camtowatervec =  CAMPOS - position;\n" //make the vector from the position of the fragment to cam pos
		"	float fresnelcoeff = abs(dot(n,normalize(camtowatervec)));\n"//fresnal coefficient
		

		//create the grid
		"	vec2 uv = texCoord * 24.0;\n" //get the uv coordinate, create the grid, get the points
		"	vec2 currgridid = floor(uv);\n"
		"	vec2 currentgridcoord = fract(uv);\n"
		"	currentgridcoord = currentgridcoord - 0.5;\n"
		" 	vec2 redgriduv = currentgridcoord;\n"
		"	redgriduv = abs(redgriduv);\n"
		"	float distedge = 10.0 * max(redgriduv.x, redgriduv.y);\n"
		"	float pointsongrid = 0.0;\n"
		"	float mindist = 100.0;\n"
		
		//"	vec3 redgridcolor = vec3(smoothstep(0.9,1.0,distedge),0.0,0.0);\n"//SHOWS THE GRIDLINE

		"	for(float i = -1.0; i <= 1.0; i++){\n"
		"		for(float j = -1.0; j <= 1.0; j++){\n"
		"			vec2 adjgridcoords = vec2(i, j);\n"
		"			vec2 pointonadjgrid = adjgridcoords;\n"
		
		"			vec2 noise = noise2x2(currgridid + adjgridcoords);\n"
    
		"			pointonadjgrid = adjgridcoords + sin(TIME * noise)* 0.5;\n"
		"			float dist = length(currentgridcoord - pointonadjgrid);\n"
		"			mindist = min(dist, mindist);\n"
		"			pointsongrid += smoothstep(0.95, 0.96, 1.0-dist);\n"
		"	}\n"
		"}\n"
		
		//"	vec3 pointsongridcolor = vec3(pointsongrid);\n" //SHOWS THE POINTS ON THE GRID
		
		"	vec3 oceanoverlay = vec3(smoothstep(0.0,1.0,mindist)) * vec3(1.0,0.9,0.4);\n" //the wave overlay color
		"	float fog = min(((objclip * postrans).z/threshold),1.0);\n" //create the fog
		"	vec4 albedo = texture(TEX, texCoord) * color;\n"
		"	fragColor = vec4( mix( 1 - (1 - oceanoverlay)*(1 - albedo.xyz) * fresnelcoeff , oceanshade,fog), 1.0);\n"
		"	fragColor = vec4(vec3(fresnelcoeff),1.0f);"
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
	OBJECT_TO_LIGHT_mat4x3 = glGetUniformLocation(program, "OBJECT_TO_LIGHT");
	OBJECT_TO_WORLD_mat4 = glGetUniformLocation(program, "OBJECT_TO_WORLD");
	NORMAL_TO_LIGHT_mat3 = glGetUniformLocation(program, "NORMAL_TO_LIGHT");

	LIGHT_TYPE_int = glGetUniformLocation(program, "LIGHT_TYPE");
	LIGHT_LOCATION_vec3 = glGetUniformLocation(program, "LIGHT_LOCATION");
	LIGHT_DIRECTION_vec3 = glGetUniformLocation(program, "LIGHT_DIRECTION");
	LIGHT_ENERGY_vec3 = glGetUniformLocation(program, "LIGHT_ENERGY");
	LIGHT_CUTOFF_float = glGetUniformLocation(program, "LIGHT_CUTOFF");
	CAMPOS_vec3 = glGetUniformLocation(program, "CAMPOS");
	CAMROT_vec3 = glGetUniformLocation(program, "CAMROT");
	TIME_float =  glGetUniformLocation(program, "TIME");
	


	GLuint TEX_sampler2D = glGetUniformLocation(program, "TEX");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now


}

WaveTextureProgram::~WaveTextureProgram() {
	glDeleteProgram(program);
	program = 0;
}

