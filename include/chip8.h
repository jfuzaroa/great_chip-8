#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <GLFW/glfw3.h>

#define CHIP8_FONT_PATH "../resrc/chip8_font.c8f"

#define CHIP8_MEM_SIZE 4096
#define CHIP8_GFX_RES_WIDTH 64
#define CHIP8_GFX_RES_HEIGHT 32
#define CHIP8_KEY_SIZE 16

#define CHIP8_PUTS(MSG) \
	puts("great_chip-8: " MSG "")

#define CHIP8_FPUTS(STRM, MSG)									\
	fputs("great_chip-8::" MSG "\n", (FILE*) { 0 } = STRM)

#define CHIP8_PERROR(ERR_MSG) \
	perror("great_chip-8::ERROR: " ERR_MSG "")

typedef uint8_t chip8_byte;
typedef uint16_t chip8_word;

/*
 * @brief Contains register bank indices for register array.
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
	chip8_byte mem[CHIP8_MEM_SIZE];
	chip8_byte gfx[CHIP8_GFX_RES_HEIGHT][CHIP8_GFX_RES_WIDTH]; /* pixel array */
	chip8_byte keys[CHIP8_KEY_SIZE];

	chip8_byte dly_tmr; /* used for timing events */
	chip8_byte snd_tmr; /* used for sound effects */
} chip8_vm;

typedef enum chip8_return_code {
	CHIP8_SUCCESS,
	CHIP8_FAILURE
} chip8_rc;

chip8_rc chip8_gfx_init(GLFWwindow*);

/*
 * @brief Allocates memory for new chip-8 object.
 */
inline chip8_vm* chip8_new(void)
{
	return calloc(1, sizeof(chip8_vm));
}

/*
 * @brief Destroys existing chip-8 object.
 */
inline void chip8_destroy(chip8_vm chip8[static 1])
{
	if (chip8) { *chip8 = (chip8_vm) { 0 }; }
}

/*
 * @brief Read file contents into the chip-8 object's memory.
 */
inline int chip8_read(chip8_vm chip8[static 1], size_t const size,
		FILE* const chip8_f)
{
	return size == fread(&chip8->mem[0x200], size, 1, chip8_f);
}

/*
 * @brief Read chip-8 font data into chip-8 object's memory.
 */
inline int chip8_font_read(chip8_vm chip8[static 1], size_t const size,
		FILE* const chip8_f)
{
	return size == fread(&chip8->mem[0], size, 1, chip8_f);
}

#endif
