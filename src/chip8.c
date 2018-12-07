#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_istr.h"

chip8_vm* chip8_new(void);
void chip8_destroy(chip8_vm[static 1]);
int chip8_read(chip8_vm[static 1], size_t const, FILE* const);
int chip8_font_read(chip8_vm[static 1], size_t const, FILE* const);

/*
 * @brief Loads the chip-8 rom file into memory.
 */
static chip8_rc chip8_load(chip8_vm chip8[static 1],
		char const chip8_fp[static 1])
{
	FILE* const chip8_rom = fopen(chip8_fp, "rb");
	size_t file_size;

	if (!chip8_rom && !fseek(chip8_rom, 0, SEEK_END)) {
		goto ERROR;
	}

	file_size = ftell(chip8_rom);
	if (!fseek(chip8_rom, 0, SEEK_SET)
			&& chip8_read(chip8, file_size, chip8_rom)
			&& !fclose(chip8_rom)) {
		chip8->pc = 0x200;
		chip8->sp = 0xEA0;
		chip8->idx = 0;
		return CHIP8_SUCCESS;
	}

ERROR:
	return CHIP8_FAILURE;
}

/*
 * @brief Loads the 4x5 chip-8 bitmap font data into memory.
 *
 * This function loads the chip-8 bitmap font data that is used for
 * drawing hexadecimal character sprites (from 0x0 to 0xF).
 * Although there is room for a 4x5 font set, the implementation
 * utilizes only a 3x5 dimension.
 */
chip8_rc chip8_font_load(chip8_vm chip8[static 1])
{
	FILE* const chip8_font = fopen(CHIP8_FONT_PATH, "rb");
	size_t file_size;

	if (!chip8_font && !fseek(chip8_font, 0, SEEK_END)) {
		goto ERROR;
	}

	file_size = ftell(chip8_font);
	if (!fseek(chip8_font, 0, SEEK_SET)
			&& chip8_font_read(chip8, file_size, chip8_font)
			&& !fclose(chip8_font)) {
		return CHIP8_SUCCESS;
	}

ERROR:
	return CHIP8_FAILURE;
}

/*
 * @brief Fetch next instruction from loaded chip-8 file.
 */
static chip8_word chip8_fetch(chip8_vm chip8[static 1])
{
	chip8_word istr = chip8->mem[chip8->pc];
	istr <<= 8;
	istr |= chip8->mem[chip8->pc+1];
	return istr;
}

/*
 * @brief Disassemble next instruction and execute with corresponding function.
 */
static chip8_rc chip8_execute(chip8_vm chip8[static 1])
{
	chip8_opcode opcode = chip8_disassemble(chip8->istr);

	if (opcode == NOP) {
		return CHIP8_FAILURE;
	}
	chip8_instruction_set[opcode](chip8);
	return CHIP8_SUCCESS;
}

int main(int argc, char* argv[argc+1])
{
	chip8_vm chip8_obj;
	chip8_vm* const chip8 = &chip8_obj;
	GLFWwindow* chip8_window = (void*) 0;
	srand((unsigned) (time(NULL)));

	if (!argv[1]) {
		CHIP8_PUTS("Missing file operand\n"
				"Try 'great-chip-8 -h' for help");
		return EXIT_FAILURE;
	} else if (!chip8_load(chip8, argv[1])) {
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

	/* fetch, decode, execute */
	while (!glfwWindowShouldClose(chip8_window)) {
		chip8->istr = chip8_fetch(chip8);
		if (chip8_execute(chip8)) {
			return EXIT_FAILURE;
		}
	}
}
