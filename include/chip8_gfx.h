#ifndef CHIP8_GFX
#define CHIP8_GFX

#include <GLFW/glfw3.h>

#define CHIP8_VERT_SHADER_PATH "../src/shaders/chip8_shader.v.glsl"
#define CHIP8_FRAG_SHADER_PATH "../src/shaders/chip8_shader.f.glsl"

#define CHIP8_DEFAULT_RES_SCALE 12.5

/*
 * @brief Chip-8 rendering structure.
 */
typedef struct chip8_renderer {
	GLuint shader_program;
	GLuint vertex_array;
} chip8_renderer;

chip8_rc chip8_init_gfx(GLFWwindow*, chip8_renderer*, const double);
void chip8_render(const chip8_renderer[const static 1]);

#endif