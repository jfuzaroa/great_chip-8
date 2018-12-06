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

/*
 * @brief chip8 game loader
 */
static chip8_rc chip8_load(chip8_vm chip8[static 1],
		char const chip8_fp[static 1])
{
	FILE* const chip8_f = fopen(chip8_fp, "rb");
	size_t size;

	if (!chip8_f && !fseek(chip8_f, 0, SEEK_END)) {
		goto ERROR;
	}

	size = ftell(chip8_f);
	if (!fseek(chip8_f, 0, SEEK_SET)
			&& chip8_read(chip8, size, chip8_f)
			&& !fclose(chip8_f)) {
		chip8->pc = 0x200;
		chip8->sp = 0xEA0;
		chip8->idx = 0;
		return CHIP8_SUCCESS;
	}

ERROR:
	return CHIP8_FAILURE;
}

/*
 * @brief fetch instruction from loaded chip8 file
 */
static chip8_word chip8_fetch(chip8_vm chip8[static 1])
{
	chip8_word istr = chip8->mem[chip8->pc];
	istr <<= 8;
	istr |= chip8->mem[chip8->pc+1];
	return istr;
}

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
		puts("great-chip-8: missing file operand\n"
				"Try 'great-chip-8 -h' for help");
		return EXIT_FAILURE;
	} else if (!chip8_load(chip8, argv[1])) {
		perror("great_chip-8: failed to load ROM");
		return EXIT_FAILURE;
	}

	/* initialize graphics and create window */
	if (!chip8_gfx_init(chip8, chip8_window)) {
		fputs("great_chip-8: OpenGL initialization failed\n", stderr);
		return EXIT_FAILURE;
	}

	/* fetch, decode, execute */
	while (!glfwWindowShouldClose(chip8_window)) {
		chip8->istr = chip8_fetch(chip8);
		if (chip8_execute(chip8)) {
			return CHIP8_SUCCESS;
		}
	}

	return EXIT_SUCCESS;
}
