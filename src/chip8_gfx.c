#include <stdlib.h>
#include <stdio.h>
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
	CHIP8_FPUTS(stderr, "ERROR::GLFW::");
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
 * @brief Creates new render object and corresponding shader program.
 */
static chip8_rc chip8_init_renderer(chip8_renderer* renderer)
{
	renderer = calloc(1, sizeof(renderer));

	if (!renderer) {
		CHIP8_FPUTS(stderr,
				"ERROR::OpenGL::RENDERER: Memory allocation failed");
		return CHIP8_FAILURE;
	}

	renderer->shader_program = glCreateProgram();

	if (renderer->shader_program) {
		CHIP8_FPUTS(stderr,
				"ERROR::OpenGL::GLSL::PROGRAM: Shader program creation failed");
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
	const char* shader_src;
	const GLuint shader = glCreateShader(shader);
	chip8_rc status = CHIP8_SUCCESS;

	if (!shader) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLSL: Shader creation failed");
		status = CHIP8_FAILURE;
	} else if (!chip8_load_shader(shader_path, shader_src)) {
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

	const GLfloat vertices[] = {
			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0
	};

	const GLsizei indices[] = {
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
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
			(GLvoid*) 0);

	/* unbind buffer and array objects */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &element_buffer);
}

/*
 * @brief Initialize OpenGL context, create window, and compile shader program.
 */
chip8_rc chip8_init_gfx(GLFWwindow* window, chip8_renderer* renderer,
		const double scale)
{
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

	window = glfwCreateWindow((int) (scale * CHIP8_GFX_RES_WIDTH),
			(int) (scale * CHIP8_GFX_RES_HEIGHT), "great_chip-8", NULL, NULL);

	if (!window) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLFW: Window creation failed");
		goto ERROR;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, chip8_fb_size_callback);
	glfwSwapInterval(1);
	glewExperimental = GL_TRUE;

	if (GLEW_OK != glewInit()) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLEW: Initialization failed");
		goto ERROR;
	}

	if (!chip8_init_renderer(renderer)) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::RENDERER: Initialization failed");
		goto ERROR;
	}

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

	chip8_init_render_data();
	return CHIP8_SUCCESS;

ERROR:
	glfwDestroyWindow(window);
	glfwTerminate();
	return CHIP8_FAILURE;
}

/* TODO */
void chip8_render(const chip8_renderer renderer[const static 1])
{
}
