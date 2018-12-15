#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_istr.h"
#include "chip8_dbg.h"

chip8_word chip8_fetch(chip8_vm chip8[static 1]);

/*
 * @brief Disassemble next instruction and execute with corresponding function.
 */
static chip8_rc chip8_execute(chip8_vm chip8[static 1])
{
	chip8_opcode opcode = chip8_disassemble(chip8->istr);

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
	static chip8_vm chip8_obj;
	static chip8_vm* const chip8 = &chip8_obj;
	static GLFWwindow* chip8_window;
	srand((unsigned) (time(NULL)));

	if (!(argc >= 2)) {
		CHIP8_PUTS("Missing ROM file\n"
				"Try 'great_chip-8 --help' for help");
		return EXIT_SUCCESS;
	} else if ('-' == argv[1][0]) {
		if (!strcmp(argv[1], "--help")) {
			CHIP8_PUTS("Usage: \"great_chip-8 -[OPTIONS] [ROM]\"");
			return EXIT_SUCCESS;
		}
		rom_index = 2;
		flag_index = 1;
	}

	if (!chip8_load(chip8, argv[rom_index])) {
		CHIP8_PERROR("Failed to load ROM");
		return EXIT_FAILURE;
	} else if (!chip8_font_load(chip8)) {
		CHIP8_PERROR("Failed to load font data");
		return EXIT_FAILURE;
	}

	/* initialize graphics and create window */
	if (!chip8_gfx_init(chip8_window)) {
		CHIP8_FPUTS(stderr, "ERROR: OpenGL initialization failed");
		return EXIT_FAILURE;
	}

	/* fetch-execute cycle */
	while (!glfwWindowShouldClose(chip8_window)) {
		chip8->istr = chip8_fetch(chip8);
		if (chip8_execute(chip8)) {
			CHIP8_FPUTS(stderr, "ERROR: chip-8 execution failed,\
					this shouldn't happen");
			return EXIT_FAILURE;
		}
		chip8_process_input(chip8, chip8_window);
	}
}
