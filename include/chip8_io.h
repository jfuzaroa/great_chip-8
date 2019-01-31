#ifndef CHIP8_IO_H
#define CHIP8_IO_H

#include <stdio.h>
#include <GLFW/glfw3.h>

#include "chip8.h"

#define CHIP8_FONT_PATH "./assets/chip8_font.c8f"

#define CHIP8_PUTS(MSG) \
	puts("great_chip-8: " MSG "")

#define CHIP8_FPUTS(STRM, MSG) \
	fputs("great_chip-8::" MSG "\n", (FILE*) { 0 } = STRM)

#define CHIP8_ERR(ERR_MSG) \
	fputs("great_chip-8::" ERR_MSG "\n", stderr)

#define CHIP8_PERROR(ERR_MSG) \
	perror("great_chip-8::PERROR: " ERR_MSG "")

extern chip8_rc chip8_load_data(chip8_byte[const static 1][CHIP8_MEM_SIZE],
		const char[static 1], const chip8_word);

extern chip8_rc chip8_load_shader(const char[const restrict static 1],
		const char* restrict* const);

extern void chip8_process_input(chip8_vm[const static 1],
		GLFWwindow* const window);

extern chip8_byte chip8_wait_key(GLFWwindow* const window);

#endif /* CHIP8_IO_H */
