#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define CHIP8_MEM_SIZE 0xFFFU
#define CHIP8_STACK_SIZE 24U
#define CHIP8_RES_WIDTH 64U
#define CHIP8_RES_HEIGHT 32U
#define CHIP8_KEY_SIZE 0xFU

/*
 * @brief contains register indices for register array
 */
enum register_unit {
	V0, V1, V2, V3,
	V4, V5, V6, V7,
	V8, V9, VA, VB,
	VC, VD, VE, VF,
	REG_UNIT_SIZE
};

/* 
 * @brief chip8 virtualized as a structure
 */
typedef struct chip8_v {

	uint16_t pc; // program counter
	uint16_t ind; // index register
	uint16_t opcode; // current opcode
	uint16_t stack[CHIP8_STACK_SIZE]; // memory stack

	uint8_t registers[REG_UNIT_SIZE]; // register unit array
	uint8_t memory[CHIP8_MEM_SIZE];
	uint8_t gfx[CHIP8_RES_WIDTH][CHIP8_RES_HEIGHT]; // pixel array (screen)
	uint8_t keyboard[CHIP8_KEY_SIZE];

	uint8_t delay_timer; // used for timing events in games 
	uint8_t sound_timer; // used for sound effects
} chip8_v;

/*
 * @brief inline chip8 constructor
 */
inline 
chip8_v* chip8_new(void) {
	return calloc(1, sizeof(chip8_v));
}

/*
 * @brief inline chip8 content destroyer
 */
inline 
void chip8_destroy(chip8_v chip8_ptr[static 1]) {
	if (chip8_ptr) {
		*chip8_ptr = (chip8_v const) { 0 };
	} else {
		errno = EINVAL;
		fprintf(stderr, "Chip8 destruction failed: %s\n", strerror(errno)); 
	}
}

/*
 * @brief inline chip8 game loader
 */
inline 
FILE* chip8_load_game(chip8_v chip8_ptr[static 1], char const game[static 1]) {
	if (FILE* game_file = fopen(game, "rb")) {
		fread(&chip8_ptr->memory[0x200], 0xDFFU, 1U, game_file);
		fclose(game_file);
	} else {
		perror("Chip8 game load failed:");
	}
}

#endif
