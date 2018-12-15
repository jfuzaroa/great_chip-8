#include <stdio.h>
#include <stdbool.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"

/*	 Chip-8 Keypad			Keyboard
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |1|2|3|C|			|1|2|3|4|
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |4|5|6|D|			|Q|W|E|R|
 *	   +-+-+-+-+	 =>		+-+-+-+-+
 *	   |7|8|9|E|			|A|S|D|F|
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |A|0|B|F|			|Z|X|C|V|
 *	   +-+-+-+-+			+-+-+-+-+
 */
int chip8_key_map[CHIP8_KEY_SIZE] = {
	GLFW_KEY_X,
	GLFW_KEY_1,
	GLFW_KEY_W,
	GLFW_KEY_3,
	GLFW_KEY_A,
	GLFW_KEY_2,
	GLFW_KEY_D,
	GLFW_KEY_Q,
	GLFW_KEY_S,
	GLFW_KEY_E,
	GLFW_KEY_Z,
	GLFW_KEY_C,
	GLFW_KEY_4,
	GLFW_KEY_R,
	GLFW_KEY_F,
	GLFW_KEY_V
};

bool chip8_read(chip8_vm[static 1], size_t const, FILE* const);
bool chip8_font_read(chip8_vm[static 1], size_t const, FILE* const);
void chip8_process_input(chip8_vm[static 1], GLFWwindow*);

/*
 * @brief Loads the chip-8 rom file into memory.
 */
chip8_rc chip8_load(chip8_vm chip8[static 1], char const chip8_fp[static 1])
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
