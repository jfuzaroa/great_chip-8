#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "chip8.h"
#include "chip8_inst.h"

chip8_vm* chip8_new(void);
void chip8_destroy(chip8_vm[static 1]);
int chip8_read(chip8_vm[static 1], size_t const, FILE* const);

/*
 * @brief chip8 game loader
 */
static chip8_rc chip8_load(chip8_vm chip8[static 1],
		char const chip8_fp[static 1])
{
	FILE* const chip8_f = fopen(chip8_fp, "rb");
	size_t size;

	if (!chip8_f && !fseek(chip8_f, 0, SEEK_END)) {
		goto ERROR;
	}

	size = ftell(chip8_f);
	if (!fseek(chip8_f, 0, SEEK_SET)
			&& chip8_read(chip8, size, chip8_f)
			&& !fclose(chip8_f)) {
		chip8->pc = 0x200;
		chip8->idx = 0;
		return CHIP8_SUCCESS;
	}

ERROR:
	return CHIP8_FAILURE;
}

/*
 * @brief fetch instruction from loaded chip8 file
 */
static chip8_word chip8_fetch(chip8_vm chip8[static 1])
{
	chip8_word inst = chip8->mem[chip8->pc];
	inst <<= 8;
	inst |= chip8->mem[chip8->pc+1];
	chip8->pc += 2;
	return inst;
}

static chip8_opcode chip8_disassemble(chip8_word inst_word)
{
	chip8_byte lend = (inst_word & 0x00FF);
	chip8_byte bend = (inst_word & 0xFF00) >> 8;

	switch (bend & 0xF0) {
		case 0x00: {
			switch(lend) {
				case 0xE0: return CLS;
				case 0xEE: return RTS;
				default: return RCA;
			}
		}
		case 0x10: return JMP;
		case 0x20: return CALL;
		case 0x30: return SKPEI;
		case 0x40: return SKPNEI;
		case 0x50: return SKPE;
		case 0x60: return MOVI;
		case 0x70: return ADDI;
		case 0x80: {
			switch(lend & 0x0F) {
				case 0x00: return MOV;
				case 0x01: return OR;
				case 0x02: return AND;
				case 0x03: return XOR;
				case 0x04: return ADD;
				case 0x05: return SUB;
				case 0x06: return SHFR;
				case 0x07: return SUBB;
				case 0x0E: return SHFL;
			}
		}
		case 0x90: return SKPNE;
		case 0xA0: return MIV;
		case 0xB0: return JMPO;
		case 0xC0: return RNDMSK;
		case 0xD0: return DRWSPT;
		case 0xE0: {
			switch(lend) {
				case 0x9E: return SKPKEY;
				case 0xA1: return SKPNKEY;
			}
		}
		case 0xF0: {
			switch(lend) {
				case 0x07: return MOVDLY;
				case 0x0A: return WTKEY;
				case 0x15: return SETDLY;
				case 0x18: return SETSND;
				case 0x1E: return IADD;
				case 0x29: return ISETSPT;
				case 0x33: return IBCD;
				case 0x55: return REGDMP;
				case 0x65: return REGLD;
			}
		}
	}
	return NOP;
}

static chip8_rc chip8_execute(chip8_vm chip8[static 1])
{
	chip8_opcode opcode = chip8_disassemble(chip8->inst_word);

	if (opcode == NOP) {
		return CHIP8_FAILURE;
	}
	chip8_instruction_set[opcode](chip8);
	return CHIP8_SUCCESS;
}

int main(int argc, char* argv[argc+1])
{
	chip8_vm chip8_obj;
	chip8_vm* const chip8 = &chip8_obj;

	if (!argv[1]) {
		puts("great-chip-8: missing file operand\n\
				Try 'great-chip-8 -h' for help");
		return EXIT_FAILURE;
	} else if (!chip8_load(chip8, argv[1])) {
		perror("Chip-8 game load failed:");
		return EXIT_FAILURE;
	}

	/* fetch, decode, execute */
	while (true) {
		chip8->inst_word = chip8_fetch(chip8);
		if (chip8_execute(chip8)) {
			return CHIP8_SUCCESS;
		}
	}

	return EXIT_SUCCESS;
}
