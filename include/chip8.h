#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CHIP8_MEM_SIZE 4096
#define CHIP8_GFX_RES_WIDTH 64
#define CHIP8_GFX_RES_HEIGHT 32
#define CHIP8_KEY_SIZE 16

typedef uint8_t chip8_byte;
typedef uint16_t chip8_word;

/*
 * @brief Return codes for chip-8 functions.
 */
typedef enum chip8_return_code {
	CHIP8_FAILURE,
	CHIP8_SUCCESS
} chip8_rc;

/*
 * @brief Register bank indices for register array.
 */
typedef enum chip8_register {
	V0, V1, V2, V3,
	V4, V5, V6, V7,
	V8, V9, VA, VB,
	VC, VD, VE, VF,
	REG_BANK_SIZE
} chip8_reg;

/* 
 * @brief Chip-8 virtual machine structure.
 */
typedef struct chip8_virtual_machine {
	chip8_word pc; /* program counter */
	chip8_word sp; /* stack pointer */
	chip8_word idx; /* index register */
	chip8_word istr; /* current instruction */

	chip8_byte regs[REG_BANK_SIZE]; /* register unit array */
	chip8_byte mem[CHIP8_MEM_SIZE]; /* memory array */
	chip8_byte gfx[CHIP8_GFX_RES_HEIGHT][CHIP8_GFX_RES_WIDTH]; /* pixel array */
	chip8_byte keys[CHIP8_KEY_SIZE]; /* input keys array */

	chip8_byte dly_tmr; /* used for timing events */
	chip8_byte snd_tmr; /* used for sound effects */
} chip8_vm;

#endif /* CHIP8_H */
