#ifndef CHIP8_GFX
#define CHIP8_GFX

#include <GLFW/glfw3.h>

#define CHIP8_VERT_SHADER_SRC "../src/shaders/chip8_shader.v.glsl"
#define CHIP8_FRAG_SHADER_SRC "../src/shaders/chip8_shader.f.glsl"

#define CHIP8_DEFAULT_RES_SCALE 12.5

chip8_rc chip8_gfx_init(GLFWwindow*);

inline chip8_rc chip8_gfx_init(GLFWwindow* window)
{
	return chip8_gfx_init(window, CHIP8_DEFAULT_RES_SCALE);
}

#endif
