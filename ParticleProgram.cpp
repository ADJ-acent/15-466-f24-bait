#include "ParticleProgram.hpp"
#include "Texture.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Load< ParticleProgram > particle_program(LoadTagEarly, []() -> ParticleProgram const * {
	ParticleProgram *ret = new ParticleProgram();
	return ret;
});

ParticleProgram::ParticleProgram() {
	//Compile vertex and fragment shaders using the convenient 'gl_compile_program' helper function:
	program = gl_compile_program(
		//vertex shader:
		"#version 330 core\n"
		"layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
		"out vec2 TexCoords;\n"
		"uniform mat4 projection;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
		"    TexCoords = vertex.zw;\n"
		"}  \n"
	,
		//fragment shader:
		"#version 330 core\n"
		"in vec2 TexCoords;\n"
		"out vec4 color;\n"
		"uniform sampler2D text;\n"
		"uniform vec3 textColor;\n"
		"void main()\n"
		"{\n"    
		"	vec4 sampled = vec4(texture(text, TexCoords));\n"
		"	color = vec4(textColor, 1.0) * sampled;\n"
		"} \n" 
	);
	//As you can see above, adjacent strings in C/C++ are concatenated.
	// this is very useful for writing long shader programs inline.

	//look up the locations of vertex attributes:
	PosTex_vec4 = glGetAttribLocation(program, "vertex");

	//look up the locations of uniforms:
	PROJECTION_mat4 = glGetUniformLocation(program, "projection");
	TexColor_vec3 = glGetUniformLocation(program, "textColor");

	GLuint TEX_sampler2D = glGetUniformLocation(program, "text");

	//set TEX to always refer to texture binding zero:
	glUseProgram(program); //bind program -- glUniform* calls refer to this program now

	glUniform1i(TEX_sampler2D, 0); //set TEX to sample from GL_TEXTURE0

	{// set VAO VBO
		constexpr float vertices[4][4] = {
			{ -0.5f, -0.5f, 0.0f, 0.0f }, // Top-left
			{  0.5f, -0.5f, 1.0f, 0.0f }, // Top-right      
			{ -0.5f,  0.5f, 0.0f, 1.0f }, // Bottom-left
			{  0.5f,  0.5f, 1.0f, 1.0f }  // Bottom-right
		};
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		// Upload the vertex data once during initialization
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Define vertex attribute layout
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now
}

ParticleProgram::~ParticleProgram() {
	glDeleteProgram(program);
	program = 0;
}

void ParticleProgram::bind_particle() const
{
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindVertexArray(VAO);
    glUseProgram(particle_program->program);

    glActiveTexture(GL_TEXTURE0);
    
	GL_ERRORS();
}

void ParticleProgram::draw_particle(const glm::mat4& clip_from_local, const GLuint texture, const glm::vec3& tint) const {
	glBindTexture(GL_TEXTURE_2D, texture);
    glUniformMatrix4fv(particle_program->PROJECTION_mat4, 1, GL_FALSE, glm::value_ptr(clip_from_local));

    glUniform3f(particle_program->TexColor_vec3, tint.x, tint.y, tint.z);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	GL_ERRORS();
}

void ParticleProgram::unbind_particle() const
{
	glBindVertexArray(0);
    
    glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
    glUseProgram(0);
	GL_ERRORS();
}