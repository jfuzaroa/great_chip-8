#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "chip8.h"
#include "chip8_inst.h"

chip8_virt_mach* chip8_new(void);
void chip8_destroy(chip8_virt_mach[static 1]);
int chip8_read(chip8_virt_mach[static 1], size_t const, FILE* const);

/*
 * @brief chip8 game loader
 */
static chip8_ret_code chip8_load(chip8_virt_mach chip8_ptr[static 1],
		char const chip8_fp[static 1])
{
	FILE* const chip8_f = fopen(chip8_fp, "rb");
	size_t size;

	if (!chip8_f && !fseek(chip8_f, 0, SEEK_END)) {
		goto ERROR;
	}

	size = ftell(chip8_f);
	if (!fseek(chip8_f, 0, SEEK_SET)
			&& chip8_read(chip8_ptr, size, chip8_f)
			&& !fclose(chip8_f)) {
		chip8_ptr->pc = 0x200;
		chip8_ptr->idx = 0;
		return CHIP8_SUCCESS;
	}

ERROR:
	perror("Chip-8 game load failed:");
	return CHIP8_FAILURE;
}

/*
 * @brief fetch instruction from loaded chip8 file
 */
static chip8_inst_size chip8_fetch_inst(chip8_virt_mach chip8_ptr[static 1])
{
	chip8_inst_size inst = chip8_ptr->mem[chip8_ptr->pc];
	inst <<= 8;
	inst |= chip8_ptr->mem[chip8_ptr->pc+1];
	chip8_ptr->pc += 2;
	return inst;
}

int main(int argc, char* argv[argc+1])
{
	chip8_virt_mach chip8_vm;
	chip8_virt_mach* const chip8 = &chip8_vm;

	if (!argv[1]) {
		puts("great-chip-8: no file specified (use -h for help)");
		goto ERROR;
	} else if (!chip8_load(chip8, argv[1])) {
		goto ERROR;
	}

	while (true) {
		chip8->inst = chip8_fetch_inst(chip8);
	}

	return EXIT_SUCCESS;

ERROR:
	return EXIT_FAILURE;
}
