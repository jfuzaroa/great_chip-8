#include <stdlib.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"

void chip8_glfw_error(int error, const char* description)
{
	CHIP8_FPUTS(stderr, "ERROR::GLFW::");
	fprintf(stderr, "%s", description);
	fputc('\n', stderr);
}

chip8_rc chip8_gfx_init(GLFWwindow* window)
{
	glfwSetErrorCallback(chip8_glfw_error);

	if (!glfwInit()) {
		CHIP8_FPUTS(stderr, "ERROR::GLFW: Initialization failed");
		goto OPENGL_INIT_ERROR;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__ /* required for macOS */
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPATA, GL_TRUE)
#endif

	window = glfwCreateWindow(800, 400, "great_chip-8", NULL, NULL);
	if (!window) {
		CHIP8_FPUTS(stderr, "ERROR::GLFW: Window creation failed");
		goto OPENGL_INIT_ERROR;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); /* with a parameter of 1 this toggles vsync on */
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit()) {
		CHIP8_FPUTS(stderr, "ERROR::GLEW: Initialization failed");
		goto OPENGL_INIT_ERROR;
	}

	return CHIP8_SUCCESS;

OPENGL_INIT_ERROR:
	glfwDestroyWindow(window);
	glfwTerminate();
	return CHIP8_FAILURE;
}
