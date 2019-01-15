#ifndef CHIP8_IO_H
#define CHIP8_IO_H

#include <stdio.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>

#include "chip8.h"

#define CHIP8_PUTS(MSG) \
	puts("great_chip-8: " MSG "")

#define CHIP8_FPUTS(STRM, MSG) \
	fputs("great_chip-8::" MSG "\n", (FILE*) { 0 } = STRM)

#define CHIP8_PERROR(ERR_MSG) \
	perror("great_chip-8::PERROR: " ERR_MSG "")

extern const int chip8_key_map[CHIP8_KEY_SIZE];

chip8_rc chip8_load_rom(chip8_vm[const static 1], const char[static 1]);
chip8_rc chip8_load_font(chip8_vm[const static 1]);
chip8_rc chip8_load_shader(const char[const restrict static 1],
		const char* restrict);

/*
 * @brief Processes keyboard mapped keyboard input using GLFW.
 */
inline void chip8_process_input(chip8_vm chip8[const static 1],
		GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
		return;
	}

	for (int i = 0; i < CHIP8_KEY_SIZE; i++) {
		if (glfwGetKey(window, chip8_key_map[i]) == GLFW_PRESS) {
			chip8->keys[i] = 1;
		} else {
			chip8->keys[i] = 0;
		}
	}
}

#endif
