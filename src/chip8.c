#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <getopt.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_istr.h"
#include "chip8_gfx.h"
#include "chip8_dbg.h"

static GLFWwindow* window;

static chip8_vm* chip8_new_vm(void)
{
	chip8_vm* chip8 = calloc(1, sizeof(*chip8));

	if (!chip8) {
		CHIP8_ERR("ERROR::Memory allocation failed");
		return NULL;
	}
	chip8->pc = 0x200;
	chip8->sp = 0xEA0;
	chip8->idx = 0;
	return chip8;
}

static chip8_rc chip8_init_vm(chip8_vm** const chip8,
		const char* argv[static 2], const size_t rom_index)
{
	*chip8 = chip8_new_vm();

	if (!*chip8) {
		CHIP8_PERROR("Virtual machine construction failed");
		return CHIP8_FAILURE;
	} else if (!chip8_load_data(&((*chip8)->mem), argv[rom_index], 0x200)) {
		CHIP8_PERROR("ROM load failed");
		return CHIP8_FAILURE;
	} else if (!chip8_load_data(&((*chip8)->mem), CHIP8_FONT_PATH, 0)) {
		CHIP8_PERROR("Font load failed");
		return CHIP8_FAILURE;
	}
	CHIP8_MEM_DUMP((*chip8)->mem);
	return CHIP8_SUCCESS;
}

/*
 * @brief Fetch next instruction from loaded chip-8 ROM.
 */
static inline chip8_word chip8_fetch(chip8_vm chip8[const static 1])
{
	chip8_word istr = chip8->mem[chip8->pc];
	istr <<= 8;
	istr |= chip8->mem[chip8->pc+1];
	return istr;
}

/*
 * @brief Disassemble next instruction and execute with corresponding function.
 */
static inline chip8_rc chip8_execute(chip8_vm chip8[const static 1])
{
	static time_t timer;
	static chip8_byte cycle;
	const chip8_opcode opcode = chip8_disassemble(chip8->istr);

	if (NOP == opcode) {
		return CHIP8_FAILURE;
	}

	chip8->dly_tmr--;
	if (cycle == 60) {
		if (chip8->dly_tmr) {
		}

		if (chip8->snd_tmr) {
			chip8->snd_tmr--;
		}
	} else if (time(NULL) == timer) {
		cycle++;
	} else {
		cycle = 0;
	}

	if (WTKEY == opcode) {
		chip8->keys[chip8_wait_key(window)] = 1;
	} else {
		chip8_istr_set[opcode](chip8);
	}

	timer = time(NULL);
	return CHIP8_SUCCESS;
}

int main(int argc, char* argv[argc+1])
{
	int exit_state = EXIT_SUCCESS;
	size_t rom_index = 1;
	size_t flag_index = 2;
	chip8_vm* chip8;
	chip8_renderer* renderer;

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

	if (!chip8_init_vm(&chip8, argv, rom_index)) {
		CHIP8_ERR("ERROR: Virtual machine initialization failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	}

	/* initialize graphics and create window */
	if (!chip8_init_gfx(&window, &renderer, CHIP8_DEFAULT_RES_SCALE)) {
		CHIP8_ERR("ERROR: OpenGL initialization failed");
		exit_state = CHIP8_FAILURE;
		goto EXIT;
	}

	/* fetch-execute cycle */
	while (!glfwWindowShouldClose(window)) {
		chip8_process_input(chip8, window);
		chip8->istr = chip8_fetch(chip8);

		if (!chip8_execute(chip8)) {
			CHIP8_ERR("ERROR: chip-8 execution failed, this shouldn't happen");
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
