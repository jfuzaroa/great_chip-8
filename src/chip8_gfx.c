#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_gfx.h"

/*
 * @brief GLFW error callback function.
 */
void chip8_glfw_error(int error, const char* description)
{
	fputs("ERROR::OpenGL::GLFW::", stderr);
	fprintf(stderr, "%s\n", description);
}

/*
 * @brief GLFW framebuffer size callback function.
 */
void chip8_fb_size_callback(GLFWwindow* window,
		const GLsizei width, const GLsizei height)
{
	glViewport(0, 0, width, height);
}

/*
 * @brief Creates new renderer object along with the including shader program.
 */
static chip8_renderer* chip8_new_renderer(void)
{
	chip8_renderer* renderer = calloc(1, sizeof(*renderer));

	if (!renderer) {
		CHIP8_FPUTS(stderr,
		            "ERROR::OpenGL::RENDERER: Memory allocation failed");
		return NULL;
	}
	renderer->shader_program = glCreateProgram();

	if (!renderer->shader_program) {
		CHIP8_FPUTS(stderr,
		            "ERROR::OpenGL::GLSL::PROGRAM: Shader program creation failed");
		free(renderer);
		return NULL;
	}
	return renderer;
}

/*
 * @brief Attempt compilation of GLSL shader given source code as string.
 */
static chip8_rc chip8_compile_shader(const GLuint shader,
		const char shader_src[static 1])
{
	GLint compile_status;
	GLsizei error_log_length;
	const GLchar* error_log;

	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if (GL_FALSE == compile_status) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_log_length);
		error_log = calloc(1, error_log_length);

		if (error_log) {
			glGetShaderInfoLog(shader, error_log_length, NULL, error_log);
			fputs(error_log, stderr);
			fputc('\n', stderr);
			free(error_log);
		} else {
			CHIP8_PERROR("Allocation failed for shader compilation error log");
		}
		return CHIP8_FAILURE;
	}

	return CHIP8_SUCCESS;
}

/*
 * @brief Load and compile GLSL shader.
 */
static chip8_rc chip8_init_shader(const char shader_path[const static 1],
		const GLenum shader_type, const GLuint program)
{
	char* shader_src;
	const GLuint shader = glCreateShader(shader_type);
	chip8_rc status = CHIP8_SUCCESS;

	if (!shader) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLSL: Shader creation failed");
		status = CHIP8_FAILURE;
	} else if (!chip8_load_shader(shader_path, &shader_src)) {
		CHIP8_PERROR("GLSL load failed");
		status = CHIP8_FAILURE;
	} else if (!chip8_compile_shader(shader, shader_src)) {
		if (GL_VERTEX_SHADER == shader_type) {
			CHIP8_FPUTS(stderr,
					"ERROR::OpenGL::GLSL::VERTEX: Compilation failed");
		} else {
			CHIP8_FPUTS(stderr,
					"ERROR::OpenGL::GLSL::FRAGMENT: Compilation failed");
		}
		status = CHIP8_FAILURE;
	}
	free(shader_src);

	if (CHIP8_SUCCESS == status) {
		glAttachShader(program, shader);
	}
	glDeleteShader(shader);
	return status;
}

/*
 * @brief Link compiled shaders and create shader program.
 */
static chip8_rc chip8_link_gfx_program(const GLuint program)
{
	GLint link_status;
	GLsizei error_log_length;
	const GLchar* error_log;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);

	if (GL_FALSE == link_status) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &error_log_length);
		error_log = calloc(1, error_log_length);

		if (error_log) {
			glGetProgramInfoLog(program, error_log_length, NULL, error_log);
			fputs(error_log, stderr);
			fputc('\n', stderr);
			free(error_log);
		} else {
			CHIP8_PERROR("Allocation failed for shader linking error log");
		}
		return CHIP8_FAILURE;
	}

	return CHIP8_SUCCESS;
}

/*
 * @brief Initialize OpenGL vertex render data.
 */
static void chip8_init_render_data(chip8_renderer renderer[const static 1])
{
	GLuint vertex_buffer;
	GLuint element_buffer;

	const GLfloat vertices[8] = {
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
	};

	const GLsizei indices[6] = {
			0, 1, 2,
			1, 2, 3
	};

	/* generate VAO, VBO, and EBO for rendering */
	glGenVertexArrays(1, &renderer->vertex_array);
	glGenBuffers(1, &vertex_buffer);
	glGenBuffers(1, &element_buffer);

	/* bind render object's VAO */
	glBindVertexArray(renderer->vertex_array);

	/* bind vertex buffer and move vertex data into memory */
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/* bind element buffer and move index data into memory */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
			GL_STATIC_DRAW);

	/* interpret vertex and index data on bound vertex array */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
			(GLvoid*) 0);

	/* unbind vertex array and set buffer for deletion */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &element_buffer);

	renderer->model_location = glGetUniformLocation(renderer->shader_program,
	                                                "model");
	renderer->projection[0]  =  2.0f / renderer->width;
	renderer->projection[5]  = -2.0f / renderer->height;
	renderer->projection[13] = -1.0f;
	renderer->projection[14] =  1.0f;
	renderer->projection[15] =  1.0f;

	renderer->model[0]  = renderer->scale;
	renderer->model[5]  = renderer->scale;
	renderer->model[15] = 1.0f;

	memcpy(renderer->sprite_color, (const GLfloat[3]){1.0f, 1.0f, 1.0f},
	       sizeof(renderer->sprite_color));

	glUseProgram(renderer->shader_program);
	glUniformMatrix4fv(
			glGetUniformLocation(renderer->shader_program, "projection"),
			1, GL_FALSE, renderer->projection);
	glUniform3fv(
			glGetUniformLocation(renderer->shader_program, "sprite_color"),
			1, renderer->sprite_color);
}

/*
 * @brief Initialize OpenGL context, create window, and compile shader program.
 */
chip8_rc chip8_init_gfx(GLFWwindow** const window_ptr,
		chip8_renderer** const renderer_ptr, const GLfloat scale)
{
	GLFWwindow* window;
	chip8_renderer* renderer;

	glfwSetErrorCallback(chip8_glfw_error);

	if (!glfwInit()) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLFW: Initialization failed");
		goto ERROR;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPATA, GL_TRUE)
#endif

	*window_ptr = glfwCreateWindow((int) (scale * CHIP8_GFX_RES_WIDTH),
			(int) (scale * CHIP8_GFX_RES_HEIGHT), "great_chip-8", NULL, NULL);

	if (!*window_ptr) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLFW: Window creation failed");
		goto ERROR;
	}
	window = *window_ptr;
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, chip8_fb_size_callback);
	glfwSwapInterval(1);
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLEW: Initialization failed");
		goto ERROR;
	}

	*renderer_ptr = chip8_new_renderer();
	if (!*renderer_ptr) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::RENDERER: Initialization failed");
		goto ERROR;
	}
	renderer = *renderer_ptr;
	renderer->scale = scale;
	renderer->width = scale * CHIP8_GFX_RES_WIDTH;
	renderer->height = scale * CHIP8_GFX_RES_HEIGHT;

	if (!chip8_init_shader(CHIP8_VERT_SHADER_PATH, GL_VERTEX_SHADER,
			renderer->shader_program)
		|| !chip8_init_shader(CHIP8_FRAG_SHADER_PATH, GL_FRAGMENT_SHADER,
			renderer->shader_program)) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLSL: Initialization failed");
		goto ERROR;
	}

	if (!chip8_link_gfx_program(renderer->shader_program)) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLSL::PROGRAM:: Linking failed");
		goto ERROR;
	}
	chip8_init_render_data(renderer);
	return CHIP8_SUCCESS;

ERROR:
	glfwDestroyWindow(window);
	glfwTerminate();
	return CHIP8_FAILURE;
}

static void chip8_draw_sprite(chip8_renderer renderer[const static 1],
		const GLuint x, const GLuint y)
{
	renderer->model[12] =  renderer->scale * x;
	renderer->model[13] = -renderer->scale * y;
	glUniformMatrix4fv(renderer->model_location, 1, GL_FALSE, renderer->model);
	glBindVertexArray(renderer->vertex_array);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid *) 0);
}

void chip8_render(const chip8_vm chip8[const static 1],
		chip8_renderer renderer[const static 1])
{
	glUseProgram(renderer->shader_program);
	glClear(GL_COLOR_BUFFER_BIT);

	for (chip8_byte i = 0; i <  CHIP8_GFX_RES_HEIGHT; i++) {
		for (chip8_byte j = 0; j < CHIP8_GFX_RES_WIDTH; j++) {
			if (chip8->gfx[i][j]) {
				chip8_draw_sprite(renderer, j, i);
			}
		}
	}
}
