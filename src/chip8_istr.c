/*
 * @file chip8_istr.c
 * @brief Implements the chip-8 disassembler and opcode function definitions.
 *
 * This contains the chip-8 disassembler for decoding instructions into their
 * corresponding functions. This also contains the opcode function definitions
 * and instruction set array with the function pointers used by the main 
 * fetch-execute cycle.
 * Function descriptions refer to variables defined in the chip-8 object
 * structure.
 *
 * @author Jonathan Alencar
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GLFW/glfw3.h>

#include "chip8.h"
#include "chip8_io.h"
#include "chip8_istr.h"
#include "chip8_dbg.h"

/*
 * @brief Disassembles chip-8 instruction into corresponding opcode.
 */
chip8_opcode chip8_disassemble(const chip8_word istr_word)
{
	const chip8_byte lend = (istr_word & 0x00FF);  /* little end */
	const chip8_byte bend = (istr_word & 0xFF00) >> 8;  /* big end */

	switch (bend & 0xF0) {
		case 0x00: {
			switch(lend) {
				case 0xE0: return CLS;
				case 0xEE: return RET;
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
		default: return NOP;
	}
}

/*
 * @brief Calls RCA 1802 program at address NNN (not required for most ROMs).
 * 0x0NNN
 */
void chip8_RCA(chip8_vm chip8[const static 1])
{
	CHIP8_ISTR_LOG("(0x0%03X) RCA %u", chip8->istr & 0x0FFF,
			chip8->istr & 0x0FFF);
    CHIP8_ERR("ERROR: RCA opcode executed, this shouldn't happen");
}

/*
 * @brief Clears the screen.
 * 0x00E0
 */
void chip8_CLS(chip8_vm chip8[const static 1])
{
	memset(chip8->gfx, 0, sizeof(chip8->gfx));
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x00E0) CLS");
}

/*
 * @brief Pops stack to return from subroutine.
 * 0x00EE
 */
void chip8_RET(chip8_vm chip8[const static 1])
{
    chip8->sp -= 2;
    chip8->pc = chip8->mem[chip8->sp] << 8;
    chip8->pc += chip8->mem[chip8->sp+1];
    CHIP8_ISTR_LOG("(0x00EE) RET");
}

/*
 * @brief Jumps to address at NNN.
 * 0x1NNN
 */
void chip8_JMP(chip8_vm chip8[const static 1])
{
    chip8->pc = chip8->istr & 0x0FFF;
	CHIP8_ISTR_LOG("(0x1%03X) JMP %u", chip8->istr & 0x0FFF,
			chip8->istr & 0x0FFF);
}

/*
 * @brief Pushes program counter to stack and calls subroutine at NNN.
 * 0x2NNN
 */
void chip8_CALL(chip8_vm chip8[const static 1])
{
    chip8->mem[chip8->sp] = ((chip8->pc+2) & 0xFF00) >> 8;
    chip8->mem[chip8->sp+1] = (chip8->pc+2) & 0x00FF;
    chip8->sp += 2;
    chip8->pc = chip8->istr & 0x0FFF;
	CHIP8_ISTR_LOG("(0x2%03X) CALL %u", chip8->istr & 0x0FFF,
			chip8->istr & 0x0FFF);
}

/*
 * @brief Skips next instruction if register V[X] equals NN.
 * 0x3XNN
 */
void chip8_SKPEI(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

    if (chip8->regs[regx] == (chip8->istr & 0x00FF)) {
        chip8->pc += 4;
    } else {
		chip8->pc += 2;
	}
	CHIP8_ISTR_LOG("(0x3%X%02X) SKPEI V%X, %u", regx, chip8->istr & 0x00FF,
			regx, chip8->istr & 0x00FF);
}

/*
 * @brief Skips next instruction if register V[X] does not equal NN.
 * 0x4XNN
 */
void chip8_SKPNEI(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

    if (chip8->regs[regx] != (chip8->istr & 0x00FF)) {
        chip8->pc += 4;
    } else {
		chip8->pc += 2;
	}
	CHIP8_ISTR_LOG("(0x4%X%02X) SKPNEI V%X, %u", regx, chip8->istr & 0x00FF,
			regx, chip8->istr & 0x00FF);
}

/*
 * @brief Skips next instruction if register V[X] equals V[Y].
 * 0x5XY0
 */
void chip8_SKPE(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    if (chip8->regs[regx] == chip8->regs[regy]) {
        chip8->pc += 4;
    } else {
		chip8->pc += 2;
	}
	CHIP8_ISTR_LOG("(0x5%X%X0) SKPE V%X, V%u", regx, regy, regx, regy);
}

/*
 * @brief Sets V[X] to NN.
 * 0x6XNN
 */
void chip8_MOVI(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

    chip8->regs[regx] = chip8->istr & 0x00FF;
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x6%X%02X) MOVI V%X, %u", regx, chip8->istr & 0x00FF,
			regx, chip8->istr & 0x00FF);
}

/*
 * @brief Adds NN to V[X].
 * 0x7XNN
 */
void chip8_ADDI(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

    chip8->regs[regx] += chip8->istr & 0x00FF;
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x7%X%02X) ADDI V%X, %u", regx, chip8->istr & 0x00FF,
			regx, chip8->istr & 0x00FF);
}

/*
 * @brief Sets V[X] to value of V[Y].
 * 0x8XY0
 */
void chip8_MOV(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    chip8->regs[regx] = chip8->regs[regy];
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X0) MOV V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Sets V[X] to V[X] OR V[Y].
 * 0x8XY1
 */
void chip8_OR(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    chip8->regs[regx] |= chip8->regs[regy];
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X1) OR V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Sets V[X] to V[X] AND V[Y].
 * 0x8XY2
 */
void chip8_AND(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    chip8->regs[regx] &= chip8->regs[regy];
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X2) AND V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Sets V[X] to V[X] XOR V[Y].
 * 0x8XY3
 */
void chip8_XOR(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    chip8->regs[regx] ^= chip8->regs[regy];
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X3) XOR V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Adds V[Y] to V[X] and sets V[F] to 0 or 1 if a carry occurs.
 * 0x8XY4
 */
void chip8_ADD(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    if (chip8->regs[regx] + chip8->regs[regy] < 0xFF) {
        chip8->regs[VF] = 0;
    } else {
        chip8->regs[VF] = 1;
    }
    chip8->regs[regx] += chip8->regs[regy];
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X4) ADD V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Subtracts V[Y] from V[X] and sets V[F] to 1 or 0 if a borrow occurs.
 * 0x8XY5
 */
void chip8_SUB(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    if (chip8->regs[regx] < chip8->regs[regy]) {
        chip8->regs[VF] = 0;
    } else {
        chip8->regs[VF] = 1;
    }
    chip8->regs[regx] -= chip8->regs[regy];
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X5) SUB V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Sets V[F] to the LSB of V[X] and shifts V[X] to the right by 1.
 * 0x8XY6
 */
void chip8_SHFR(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    chip8->regs[VF] = chip8->regs[regx] & 0x01;
    chip8->regs[regx] >>= 1;
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X6) SHFR V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Subtracts V[X] from V[Y] and sets V[F] to 1 or to 0 if a borrow
 * occurs.
 * 0x8XY7
 */
void chip8_SUBB(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    if (chip8->regs[regy] < chip8->regs[regx]) {
        chip8->regs[VF] = 0;
    } else {
        chip8->regs[VF] = 1;
    }
    chip8->regs[regx] = chip8->regs[regy] - chip8->regs[regx];
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%X7) SUBB V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Sets V[F] to the MSB of V[X] and shifts V[X] to the left by 1.
 * 0x8XYE
 */
void chip8_SHFL(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    chip8->regs[VF] = chip8->regs[regx] & 0x80;
    chip8->regs[regx] <<= 1;
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0x8%X%XE) SHFL V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Skips next instruction if V[X] does not equal V[Y].
 * 0x9XY0
 */
void chip8_SKPNE(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
    const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;

    if (chip8->regs[regx] != chip8->regs[regy]) {
        chip8->pc += 4;
    } else {
		chip8->pc += 2;
	}
	CHIP8_ISTR_LOG("(0x9%X%X0) SKPNE V%X, V%X", regx, regy, regx, regy);
}

/*
 * @brief Sets I to the address NNN.
 * 0xANNN
 */
void chip8_MIV(chip8_vm chip8[const static 1])
{
    chip8->idx = chip8->istr & 0x0FFF;
    chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xA%03X) MIV %u", chip8->istr & 0x0FFF,
			chip8->istr & 0x0FFF);
}

/*
 * @brief Jumps to address NNN plus V[0].
 * 0xBNNN
 */
void chip8_JMPO(chip8_vm chip8[const static 1])
{
    chip8->pc = (chip8->istr & 0x0FFF) + chip8->regs[V0];
	CHIP8_ISTR_LOG("(0xB%03X) JMPO %u", chip8->istr & 0x0FFF,
			chip8->istr & 0x0FFF);
}

/*
 * @brief Sets V[X] equal to a bitwise AND between a random number and NN.
 * 0xCXNN
 */
void chip8_RNDMSK(chip8_vm chip8[const static 1])
{
    const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

    chip8->regs[regx] = (rand() % 256) & (chip8->istr & 0x00FF);
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xC%X%02X) RNDMSK V%X, %u", regx, chip8->istr & 0x00FF,
			regx, chip8->istr & 0x00FF);
}

/*
 * @brief Draws a sprite at coordinate (V[X], V[Y]) with dimensions 8xN.
 * 0xDXYN
 */
void chip8_DRWSPT(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
	const chip8_reg regy = (chip8->istr & 0x00F0) >> 4;
	const chip8_byte hgt = chip8->istr & 0x000F;
	const chip8_byte x = chip8->regs[regx];
	const chip8_byte y = chip8->regs[regy];

	chip8->regs[VF] = 0;

	for (chip8_byte bit_row, i = 0; i < hgt; i++) {
		bit_row = chip8->mem[chip8->idx+i];
		for (chip8_byte j = 0; j < 8; j++) {
			if ((chip8_byte) ((bit_row >> (7-j)) << 7)) {
				if (chip8->gfx[y+i][x+j]) {
					chip8->regs[VF] = 1;
				}
				chip8->gfx[y+i][x+j] ^= 1;
			}
		}
	}
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xD%X%X%X) DRWSPT V%X, V%X, %u", regx, regy, hgt,
			regx, regy, hgt);
	for (int i = 0; i < hgt; i++) {
		CHIP8_ISTR_LOG("%02u: mem[%u] -> 0x%04X", i, chip8->idx+i,
				chip8->mem[chip8->idx+i]);
	}
}

/*
 * @brief Skips next instruction if key stored in V[X] is pressed.
 * 0xEX9E
 */
void chip8_SKPKEY(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	if (chip8_keys[regx]) {
		chip8->pc += 4;
	} else {
		chip8->pc += 2;
	}
	CHIP8_ISTR_LOG("(0xE%X9E) SKPKEY V%X", regx, regx);
}

/*
 * @brief Skips next instruction if key stored in V[X] is not pressed.
 * 0xEXA1
 */
void chip8_SKPNKEY(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	if (!chip8_keys[regx]) {
		chip8->pc += 4;
	} else {
		chip8->pc += 2;
	}
	CHIP8_ISTR_LOG("(0xE%XA1) SKPNKEY V%X", regx, regx);
}

/*
 * @brief Sets V[X] to the value of the delay timer.
 * 0xFX07
 */
void chip8_MOVDLY(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	chip8->regs[regx] = chip8->dly_tmr;
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X07) MOVDLY V%X", regx, regx);
}

/*
 * @brief Halts all instructions and stores next key press in V[X].
 * 0xFX0A
 */
void chip8_WTKEY(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;
	chip8_byte keys_state[CHIP8_KEY_SIZE];

	memcpy(keys_state, chip8_keys, sizeof(chip8_keys));
	glfwWaitEvents();

	for (chip8_byte i = 0; i < CHIP8_KEY_SIZE; i++) {
		if (chip8_keys[i] != keys_state[i]) {
			chip8->regs[regx] = i;
			break;
		}
	}
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X0A) WTKEY V%X", regx, regx);
}

/*
 * @brief Sets delay timer to V[X].
 * 0xFX15
 */
void chip8_SETDLY(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	chip8->dly_tmr = chip8->regs[regx];
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X15) SETDLY V%X", regx, regx);
}

/*
 * @brief Sets sound timer to V[X].
 * 0xFX18
 */
void chip8_SETSND(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	chip8->snd_tmr = chip8->regs[regx];
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X18) SETSND V%X", regx, regx);
}

/*
 * @brief Adds V[X] to I.
 * 0xFX1E
 */
void chip8_IADD(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	chip8->idx += regx;
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X1E) IADD V%X", regx, regx);
}

/*
 * @brief Sets index register to the location of the sprite character in V[X].
 * 0xFX29
 */
void chip8_ISETSPT(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	chip8->idx = regx * 5;
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X1E) ISETSPT V%X", regx, regx);
}

/*
 * @brief Stores BCD representation of V[X] in memory starting at the index.
 * 0xFX33
 */
void chip8_IBCD(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	chip8->mem[chip8->idx+0] = regx / 100;
	chip8->mem[chip8->idx+1] = regx / 10;
	chip8->mem[chip8->idx+2] = regx;
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X33) IBCD V%X", regx, regx);
}

/*
 * @brief Stores V[0] to V[X] in memory starting at the index.
 * 0xFX55
 */
void chip8_REGDMP(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	for(short i = regx; i >= 0; i--) {
		chip8->mem[chip8->idx+i] = chip8->regs[i];
	}
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X55) REGDMP V%X", regx, regx);
}

/*
 * @brief Fills V[0] to V[X] with values from memory starting at the index.
 * 0xFX65
 */
void chip8_REGLD(chip8_vm chip8[const static 1])
{
	const chip8_reg regx = (chip8->istr & 0x0F00) >> 8;

	for(short i = regx; i >= 0; i--) {
		chip8->regs[i] = chip8->mem[chip8->idx+i];
	}
	chip8->pc += 2;
	CHIP8_ISTR_LOG("(0xF%X65) REGLD V%X", regx, regx);
}

chip8_istr* chip8_istr_set[CHIP8_ISTR_SET_SIZE] = {
	[RCA]		= chip8_RCA,
	[CLS]		= chip8_CLS,
	[RET]		= chip8_RET,
	[JMP]		= chip8_JMP,
	[CALL]		= chip8_CALL,
	[SKPEI]		= chip8_SKPEI,
	[SKPNEI]	= chip8_SKPNEI,
	[SKPE]		= chip8_SKPE,
	[MOVI]		= chip8_MOVI,
	[ADDI]		= chip8_ADDI,
	[MOV]		= chip8_MOV,
	[OR]		= chip8_OR,
	[AND]		= chip8_AND,
	[XOR]		= chip8_XOR,
	[ADD]		= chip8_ADD,
	[SUB]		= chip8_SUB,
	[SHFR]		= chip8_SHFR,
	[SUBB]		= chip8_SUBB,
	[SHFL]		= chip8_SHFL,
	[SKPNE]		= chip8_SKPNE,
	[MIV]		= chip8_MIV,
	[JMPO]		= chip8_JMPO,
	[RNDMSK]	= chip8_RNDMSK,
	[DRWSPT]	= chip8_DRWSPT,
	[SKPKEY]	= chip8_SKPKEY,
	[SKPNKEY]	= chip8_SKPNKEY,
	[MOVDLY]	= chip8_MOVDLY,
	[WTKEY]		= chip8_WTKEY,
	[SETDLY]	= chip8_SETDLY,
	[SETSND]	= chip8_SETSND,
	[IADD]		= chip8_IADD,
	[ISETSPT]	= chip8_ISETSPT,
	[IBCD]		= chip8_IBCD,
	[REGDMP]	= chip8_REGDMP,
	[REGLD]		= chip8_REGLD
};
