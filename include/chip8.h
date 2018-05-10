#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CHIP8_MEM_SIZE 0xFFF
#define CHIP8_STACK_SIZE 24
#define CHIP8_DISP_RES_WIDTH 64
#define CHIP8_DISP_RES_HEIGHT 32
#define CHIP8_KEY_SIZE 0xF

typedef uint16_t chip8_inst_size;

/*
 * @brief contains register indices for register array
 */
enum chip8_register_bank {
	V0, V1, V2, V3,
	V4, V5, V6, V7,
	V8, V9, VA, VB,
	VC, VD, VE, VF,
	REG_BANK_SIZE
};

/* 
 * @brief chip8 virtual machine structure
 */
typedef struct chip8_virtual_machine {
	uint16_t pc; /* program counter */
	uint16_t idx; /* index register */
	chip8_inst_size inst; /* current instruction */
	uint16_t stack[CHIP8_STACK_SIZE]; /* memory stack */

	uint8_t regs[REG_BANK_SIZE]; /* register unit array */
	uint8_t mem[CHIP8_MEM_SIZE];
	uint8_t gfx[CHIP8_DISP_RES_WIDTH][CHIP8_DISP_RES_HEIGHT]; /* pixel array */
	uint8_t keys[CHIP8_KEY_SIZE];

	uint8_t dly_tmr; /* used for timing events */
	uint8_t snd_tmr; /* used for sound effects */
} chip8_virt_mach;

typedef enum chip8_return_codes {
	CHIP8_SUCCESS,
	CHIP8_FAILURE
} chip8_ret_code; 

/*
 * @brief chip8_vm constructor
 */
inline chip8_virt_mach* chip8_new(void)
{
	return calloc(1, sizeof(chip8_virt_mach));
}

/*
 * @brief chip8_vm content destroyer for resetting chip8_vm structure
 */
inline void chip8_destroy(chip8_virt_mach chip8_ptr[static 1])
{
	if (chip8_ptr) { *chip8_ptr = (chip8_virt_mach) { 0 }; }
}

/*
 * @brief read file contents into chip8_vm memory array
 */
inline int chip8_read(chip8_virt_mach chip8_ptr[static 1], size_t const size,
		FILE* const chip8_f)
{
	return size == fread(&chip8_ptr->mem[0x200], size, 1, chip8_f);
}

#endif
