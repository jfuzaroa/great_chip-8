#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <getopt.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_gfx.h"
#include "chip8_istr.h"
#include "chip8_dbg.h"

chip8_word chip8_fetch(chip8_vm[const static 1]);

static chip8_vm* chip8_new_vm(void)
{
	chip8_vm* chip8 = calloc(1, sizeof(*chip8));

	if (!chip8) {
		CHIP8_FPUTS(stderr, "ERROR::Memory allocation failed");
		return NULL;
	}
	chip8->pc = 0x200;
	chip8->sp = 0xEA0;
	chip8->idx = 0;
	return chip8;
}

/*
 * @brief Disassemble next instruction and execute with corresponding function.
 */
static chip8_rc chip8_execute(chip8_vm chip8[const static 1])
{
	const chip8_opcode opcode = chip8_disassemble(chip8->istr);

	if (NOP == opcode) {
		return CHIP8_FAILURE;
	}
	chip8_istr_set[opcode](chip8);
	return CHIP8_SUCCESS;
}

int main(int argc, char* argv[argc+1])
{
	size_t rom_index = 1;
	size_t flag_index = 2;
	chip8_vm* chip8;
	chip8_renderer* renderer;
	GLFWwindow* window;
	int exit_state = EXIT_SUCCESS;

	srand((unsigned) time(NULL));

	if (argc < 2) {
		CHIP8_PUTS("Missing ROM file\n"
				"Try 'great_chip-8 --help' for help.");
		return exit_state;
	} else if ('-' == argv[1][0]) {
		if (!strcmp(argv[1], "--help")) {
			CHIP8_PUTS("Usage: \"great_chip-8 -[OPTIONS] [ROM]\"");
			return exit_state;
		}

		rom_index = 2;
		flag_index = 1;
	}

	/* construct Chip-8 object */
	chip8 = chip8_new_vm();

	if (!chip8) {
		CHIP8_PERROR("Virtual machine construction failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	}

	/* load ROM and font data into memory */
	if (!chip8_load_data(&chip8->mem, argv[rom_index], 0x200)) {
		CHIP8_PERROR("ROM load failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	} else if (!chip8_load_data(&chip8->mem, CHIP8_FONT_PATH, 0)) {
		CHIP8_PERROR("Font load failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	}

	/* initialize graphics and create window */
	if (!chip8_init_gfx(&window, &renderer, CHIP8_DEFAULT_RES_SCALE)) {
		CHIP8_FPUTS(stderr, "ERROR: OpenGL initialization failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	}

	/* fetch-execute cycle */
	while (!glfwWindowShouldClose(window)) {
		chip8_process_input(chip8, window);
		chip8->istr = chip8_fetch(chip8);

		if (!chip8_execute(chip8)) {
			CHIP8_FPUTS(stderr, "ERROR: chip-8 execution failed, "
					   "this shouldn't happen");
			exit_state = CHIP8_FAILURE;
			goto EXIT;
		}
		chip8_render(chip8, renderer);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

EXIT:
	free(chip8);
	free(renderer);
	return exit_state;
}
