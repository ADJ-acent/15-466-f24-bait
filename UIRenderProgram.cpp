#include "UIRenderProgram.hpp"
#include "Texture.hpp"
#include "gl_compile_program.hpp"
#include "gl_errors.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Load< UIRenderProgram > ui_render_program(LoadTagEarly, []() -> UIRenderProgram const * {
	UIRenderProgram *ret = new UIRenderProgram();
	return ret;
});

UIRenderProgram::UIRenderProgram() {
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

	text_program = gl_compile_program(
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
		"	float sampled = texture(text, TexCoords).r;\n"
		"	color = vec4(textColor, sampled) ;\n"
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

	glUseProgram(0); //unbind program -- glUniform* calls refer to ??? now
}

UIRenderProgram::~UIRenderProgram() {
	glDeleteProgram(program);
	glDeleteProgram(text_program);
	program = 0;
	text_program = 0;
}

void UIRenderProgram::draw_ui(const Texture& texture, glm::vec2 position, glm::uvec2 drawable_size, AlignMode align, glm::vec2 scale, glm::vec3 tint, bool single_channel) const
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// for single channel textures, use the text program
	if (single_channel) {
		glUseProgram(ui_render_program->text_program);
	}
	else {
		glUseProgram(ui_render_program->program);
	}
    // activate corresponding render state
	float aspect = float(drawable_size.y) / float(drawable_size.x);
	glm::mat4 projection = glm::ortho(0.0f, 1280.0f, 0.0f, 1280.0f * aspect);
	glUniformMatrix4fv(ui_render_program->PROJECTION_mat4,  1, GL_FALSE, glm::value_ptr(projection));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

	float width = texture.width * scale.x;
	float height = texture.height * scale.y;

	glm::vec2 align_offset = get_align_offset(align);
	align_offset.x *= width;
	align_offset.y *= height;

	float absolute_position_x = position.x * 1280.0f + align_offset.x;
	float absolute_position_y =  position.y * 1280.0f * aspect + align_offset.y;

	// quad that we will draw
	float vertices[4][4] = {
		{ absolute_position_x, absolute_position_y,   0.0f, 0.0f }, // Top-left
		{ absolute_position_x + width, absolute_position_y, 1.0f, 0.0f },  // Top-right      
		{ absolute_position_x, absolute_position_y + height, 0.0f, 1.0f }, // Bottom-left
		{ absolute_position_x + width, absolute_position_y + height, 1.0f, 1.0f }, // Bottom-right
	};

	// set tint of the element
	glUniform3f(ui_render_program->TexColor_vec3, tint.x, tint.y, tint.z);

	glBindTexture(GL_TEXTURE_2D, texture.handle);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
	// render triangle strip
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_BLEND);
	glUseProgram(0);
}

glm::vec2 UIRenderProgram::get_align_offset(AlignMode align)
{
	glm::vec2 offset = glm::vec2(0);
    switch (align) {
		case AlignMode::TopLeft:
			offset = glm::vec2(0.0f, -1.0f);
			break;
		case AlignMode::Top:
			offset = glm::vec2(-0.5f, -1.0f);
			break;
		case AlignMode::TopRight:
			offset = glm::vec2(-1.0f, -1.0f);
			break;
		case AlignMode::CenterLeft:
			offset = glm::vec2(0.0f, -0.5f);
			break;
		case AlignMode::Center:
			offset = glm::vec2(-0.5f, -0.5f);
			break;
		case AlignMode::CenterRight:
			offset = glm::vec2(-1.0f, -0.5f);
			break;
		case AlignMode::BottomLeft:
			offset = glm::vec2(0.0f, 0.0f);
			break;
		case AlignMode::Bottom:
			offset = glm::vec2(-0.5f, 0.0f);
			break;
		case AlignMode::BottomRight:
			offset = glm::vec2(-1.0f, 0.0f);
			break;
	}
	return offset;
}
