#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_gfx.h"

chip8_rc chip8_gfx_init(GLFWwindow* window);

void chip8_glfw_error(int error, const char* description)
{
	CHIP8_FPUTS(stderr, "ERROR::GLFW::");
	fprintf(stderr, "%s\n", description);
}

void chip8_fb_size_callback(GLFWwindow* window, GLsizei width, GLsizei height)
{
	glViewport(0, 0, width, height);
}

/* TODO */
chip8_rc chip8_shader_init(const restrict char vert_shader_path[static 1],
		char restrict const frag_shader_path[static 1]) 
{
	char const* shader_src;
	static GLuint chip8_vert_shader = glCreateShader(GL_VERTEX_SHADER);

/*     glShaderSource(chip8_vert_shader, 1,  */
}

chip8_rc chip8_gfx_init(GLFWwindow* window, unsigned short res_scale)
{
	glfwSetErrorCallback(chip8_glfw_error);

	if (!glfwInit()) {
		CHIP8_FPUTS(stderr, "ERROR::GLFW: Initialization failed");
		goto OPENGL_INIT_ERROR;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* required for macOS */
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPATA, GL_TRUE)
#endif

	window = glfwCreateWindow(CHIP8_GFX_RES_WIDTH * res_scale,
			CHIP8_GFX_RES_HEIGHT * res_scale, "great_chip-8", NULL, NULL);
	if (!window) {
		CHIP8_FPUTS(stderr, "ERROR::GLFW: Window creation failed");
		goto OPENGL_INIT_ERROR;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, chip8_fb_size_callback);
	glfwSwapInterval(1); /* toggles v-sync on */

	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		CHIP8_FPUTS(stderr, "ERROR::GLEW: Initialization failed");
		goto OPENGL_INIT_ERROR;
	}

	chip8_shader_init(CHIP8_VERT_SHADER_PATH, CHIP8_FRAG_SHADER_PATH);

	return CHIP8_SUCCESS;

OPENGL_INIT_ERROR:
	glfwDestroyWindow(window);
	glfwTerminate();
	return CHIP8_FAILURE;
}

chip8_rc chip8_gfx_init(GLFWwindow* window)
{
	return chip8_gfx_init(window, CHIP8_DEFAULT_RES_SCALE);
}
