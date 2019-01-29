#ifndef CHIP8_GFX_H
#define CHIP8_GFX_H

#include <GLFW/glfw3.h>

#define CHIP8_VERT_SHADER_PATH "./src/shaders/chip8_shader.v.glsl"
#define CHIP8_FRAG_SHADER_PATH "./src/shaders/chip8_shader.f.glsl"

#define CHIP8_DEFAULT_RES_SCALE 12.5f

/*
 * @brief Chip-8 rendering structure.
 */
typedef struct chip8_renderer {
	GLuint shader_program; /* shader program ID */
	GLuint vertex_array; /* vertex array ID */
	GLint model_location; /* model matrix location */

	GLfloat projection[16]; /* projection matrix */
	GLfloat model[16]; /* model matrix */
	GLfloat sprite_color[3]; /* sprite color */

	GLfloat scale; /* resolution scalar */
	GLfloat width; /* resolution width */
	GLfloat height; /* resolution height */
} chip8_renderer;

extern chip8_rc chip8_init_gfx(GLFWwindow** const, chip8_renderer** const,
		const GLfloat);

extern void chip8_render(const chip8_vm[const static 1],
		chip8_renderer[const static 1]);

#endif /* CHIP8_GFX_H */
