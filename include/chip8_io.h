#ifndef CHIP8_IO_H
#define CHIP8_IO_H

#include <stdio.h>
#include <GLFW/glfw3.h>

#include "chip8.h"

#define CHIP8_KEY_SIZE 16

#define CHIP8_FONT_PATH "./assets/chip8_font.c8f"

#define CHIP8_FPUTS(STRM, MSG) \
	fputs("great_chip-8::" MSG "\n", (FILE*) { 0 } = STRM)

#define CHIP8_PUTS(MSG) \
	CHIP8_FPUTS(stdout, MSG)

#define CHIP8_ERR(ERR_MSG) \
	CHIP8_FPUTS(stderr, ERR_MSG)

#define CHIP8_PERROR(ERR_MSG) \
	perror("great_chip-8::PERROR: " ERR_MSG "")

chip8_byte chip8_keys[CHIP8_KEY_SIZE];

extern chip8_rc chip8_load_data(chip8_byte[const static 1][CHIP8_MEM_SIZE],
		const char[static 1], const chip8_word);

extern chip8_rc chip8_load_shader(const char[const restrict static 1],
		const char* restrict* const);

extern void chip8_process_input(GLFWwindow* window, int, int, int, int);

extern chip8_byte chip8_wait_key(GLFWwindow* const window);

#endif /* CHIP8_IO_H */
