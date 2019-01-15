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
const int chip8_key_map[CHIP8_KEY_SIZE] = {
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

void chip8_process_input(chip8_vm[const static 1], GLFWwindow*);

/*
 * @brief Returns the number of bytes in a file.
 */
long chip8_measure_file(FILE* chip8_file)
{
	fseek(chip8_file, 0, SEEK_END);
	return ftell(chip8_file);
}

/*
 * @brief Loads the chip-8 rom file into memory.
 */
chip8_rc chip8_load_rom(chip8_vm chip8[const static 1],
		const char chip8_fp[static 1])
{
	FILE* chip8_rom = fopen(chip8_fp, "rb");
	long file_size = chip8_measure_file(chip8_rom);

	rewind(chip8_rom);

	if (-1 != file_size
	&& fread(&chip8->mem[0x200], file_size, 1, chip8_rom) == file_size
	&& !fclose(chip8_rom)) {
		chip8->pc = 0x200;
		chip8->sp = 0xEA0;
		chip8->idx = 0;
		return CHIP8_SUCCESS;
	}

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
chip8_rc chip8_load_font(chip8_vm chip8[const static 1])
{
	FILE* chip8_font = fopen(CHIP8_FONT_PATH, "rb");
	long file_size = chip8_measure_file(chip8_font);

	rewind(chip8_font);

	if (-1 != file_size
	&& fread(&chip8->mem[0], file_size, 1, chip8_font) == file_size
	&& !fclose(chip8_font)) {
		return CHIP8_SUCCESS;
	}

	return CHIP8_FAILURE;
}

/*
 * @brief Loads shader source given by file path into a string of characters.
 */
chip8_rc chip8_load_shader(const char shader_path[const restrict static 1],
		const char* restrict shader_src)
{
	FILE* chip8_shader = fopen(shader_path, "r");
	const long file_size = chip8_measure_file(chip8_shader);

	rewind(chip8_shader);
	
	if (-1 != file_size) {
		shader_src = calloc(1, file_size + 1);

		if (shader_src != NULL
		&& fread(shader_src, file_size, 1, chip8_shader) == file_size
		&& !fclose(chip8_shader)) {
			return CHIP8_SUCCESS;
		}
	}

	return CHIP8_FAILURE;
}
