#include "chip8.h"

chip8_v* chip8_new(void);
void chip8_destroy(chip8_v[static 1]);
FILE* chip8_load_game(chip8_v[static 1], char const[static 1]);


void chip8_start(chip8_v chip8_ptr[static 1], char const game[static 1]) {
	chip8_load_game(chip8_ptr, game);
}

void chip8_stop(chip8_v chip8_ptr[static 1]);

int main(int argc, char* argv[argc+1]) {

	chip8_v* chip8 = chip8_new();

	if (chip8) {
		chip8_start(chip8, argv[1]);
	} else {
		perror("Chip8 allocation failed:");
	}

	return EXIT_SUCCESS;
}
