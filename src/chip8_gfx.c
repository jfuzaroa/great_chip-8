#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_gfx.h"

void chip8_glfw_error(int error, const char* description)
{
	CHIP8_FPUTS(stderr, "ERROR::GLFW::");
	fprintf(stderr, "%s\n", description);
}

/*
 * @brief GLFW framebuffer size callback function.
 */
void chip8_fb_size_callback(GLFWwindow* window, GLsizei width, GLsizei height)
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
	GLsizei info_log_length;
	const GLchar* info_log;

	glShaderSource(shader, 1, &shader_src, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

	if (GL_FALSE == compile_status) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);
		info_log = calloc(1, info_log_length);

		if (NULL != info_log) {
			glGetShaderInfoLog(shader, info_log_length, NULL, info_log);
			fputs(info_log, stderr);
			fputc('\n', stderr);
			free(info_log);
		}
		return CHIP8_FAILURE;
	}

	return CHIP8_SUCCESS;
}

/*
 * @brief Load and compile GLSL shader.
 */
static chip8_rc chip8_init_shader(
		const char shader_path[const restrict static 1],
		const GLenum shader_type, const GLuint program)
{
	const char* shader_src;
	const GLuint shader = glCreateShader(shader);
	chip8_rc status = CHIP8_SUCCESS;

	if (0 != shader && !chip8_load_shader(shader_path, shader_src)) {
		CHIP8_PERROR("Failed to load GLSL shader");
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

static chip8_rc chip8_link_gfx_program(const GLuint program)
{
	GLint link_status;
	GLsizei info_log_length;
	const GLchar* info_log;

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_status);

	if (GL_FALSE == link_status) {
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);
		info_log = calloc(1, info_log_length);

		if (NULL != info_log) {
			glGetProgramInfoLog(program, info_log_length, NULL, info_log);
			fputs(info_log, stderr);
			fputc('\n', stderr);
			free(info_log);
		}
		return CHIP8_FAILURE;
	}

	return CHIP8_SUCCESS;
}

/*
 * @brief Initialize OpenGL context, create window, and compile shader program.
 */
chip8_rc chip8_init_gfx(GLFWwindow* window, const double scale)
{
	GLuint chip8_gfx_program = glCreateProgram();
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

	if (0 != chip8_gfx_program
	&& !chip8_init_shader(CHIP8_VERT_SHADER_PATH, GL_VERTEX_SHADER,
			chip8_gfx_program)
	&& !chip8_init_shader(CHIP8_FRAG_SHADER_PATH, GL_FRAGMENT_SHADER,
			chip8_gfx_program)) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLSL: Initialization failed");
	}

	if (!chip8_link_gfx_program(chip8_gfx_program)) {
		CHIP8_FPUTS(stderr, "ERROR::OpenGL::GLSL::PROGRAM:: Linking failed");
	}

	return CHIP8_SUCCESS;

ERROR:
	glfwDestroyWindow(window);
	glfwTerminate();
	return CHIP8_FAILURE;
}
