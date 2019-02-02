#include <stdio.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_dbg.h"

/*	  Chip-8 Keypad         Keyboard
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |1|2|3|C|			|1|2|3|4|
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |4|5|6|D|			|Q|W|E|R|
 *	   +-+-+-+-+	  =>	+-+-+-+-+
 *	   |7|8|9|E|			|A|S|D|F|
 *	   +-+-+-+-+			+-+-+-+-+
 *	   |A|0|B|F|			|Z|X|C|V|
 *	   +-+-+-+-+			+-+-+-+-+
 */
static const int glfw_key_map[CHIP8_KEY_SIZE] = {
		[CHIP8_KEY_0] = GLFW_KEY_X,
		[CHIP8_KEY_1] = GLFW_KEY_1,
		[CHIP8_KEY_2] = GLFW_KEY_2,
		[CHIP8_KEY_3] = GLFW_KEY_3,
		[CHIP8_KEY_4] = GLFW_KEY_Q,
		[CHIP8_KEY_5] = GLFW_KEY_W,
		[CHIP8_KEY_6] = GLFW_KEY_E,
		[CHIP8_KEY_7] = GLFW_KEY_A,
		[CHIP8_KEY_8] = GLFW_KEY_S,
		[CHIP8_KEY_9] = GLFW_KEY_D,
		[CHIP8_KEY_A] = GLFW_KEY_Z,
		[CHIP8_KEY_B] = GLFW_KEY_C,
		[CHIP8_KEY_C] = GLFW_KEY_4,
		[CHIP8_KEY_D] = GLFW_KEY_R,
		[CHIP8_KEY_E] = GLFW_KEY_F,
		[CHIP8_KEY_F] = GLFW_KEY_V
};

/*
 * @brief Returns the number of bytes in a file.
 */
static inline long chip8_measure_file(const FILE chip8_file[static 1])
{
	if (chip8_file) {
		fseek(chip8_file, 0, SEEK_END);
		return ftell(chip8_file);
	}
	return -1;
}

/*
 * @brief Loads Chip-8 data into memory.
 */
chip8_rc chip8_load_data(chip8_byte chip8_mem[const static 1][CHIP8_MEM_SIZE],
		const char file_path[static 1], const chip8_word index)
{
	long file_size = -1;
	FILE* file = fopen(file_path, "rb");

	if (file) {
		file_size = chip8_measure_file(file);

		if (-1 < file_size) {
			rewind(file);

			if (fread(&((*chip8_mem)[index]), 1, file_size, file) == file_size
			    && !fclose(file)) {
				return CHIP8_SUCCESS;
			}
		}
	}
	return CHIP8_FAILURE;
}

/*
 * @brief Loads shader source given by file path into a string of characters.
 */
chip8_rc chip8_load_shader(const char shader_path[const restrict static 1],
		const char* restrict* const shader_src)
{
	FILE* chip8_shader = fopen(shader_path, "r");
	const long file_size = chip8_measure_file(chip8_shader);

	if (-1 != file_size) {
		rewind(chip8_shader);
		*shader_src = calloc(1, file_size + 1);

		if ((*shader_src) != NULL
		&& fread(*shader_src, 1, file_size, chip8_shader) == file_size
		&& !fclose(chip8_shader)) {
			return CHIP8_SUCCESS;
		}
	}
	return CHIP8_FAILURE;
}

/*
 * @brief Translates GLFW key value into Chip-8 key map index.
 */
static inline chip8_key chip8_translate_glfw_key(const int key) {
	for (int i = 0; i < CHIP8_KEY_SIZE; i++) {
		if (glfw_key_map[i] == key) {
			return i;
		}
	}
	return CHIP8_KEY_UNKNOWN;
}

/*
 * @brief Processes keyboard mapped keyboard input using GLFW.
 */
void chip8_key_callback(GLFWwindow* const window,
		const int key, const int scan_code, const int action, const int mods)
{
	int chip8_key;

	if (GLFW_KEY_ESCAPE == key &&  GLFW_PRESS == action) {
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	chip8_key = chip8_translate_glfw_key(key);

	if (GLFW_PRESS == action) {
		chip8_keys[chip8_key] = true;
		CHIP8_KEY_PRESS(chip8_key);
	} else if (GLFW_RELEASE == action) {
		chip8_keys[chip8_key] = false;
	}
}
