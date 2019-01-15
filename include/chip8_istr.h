#ifndef CHIP8_ISTR_H
#define CHIP8_ISTR_H

#include <stdlib.h>
#include <stdio.h>

typedef enum chip8_opcode {
	NOP = -1,
	RCA, CLS, RET, JMP, CALL,
	SKPEI, SKPNEI, SKPE, MOVI, ADDI,
	MOV, OR, AND, XOR, ADD,
	SUB, SHFR, SUBB, SHFL, SKPNE,
	MIV, JMPO, RNDMSK, DRWSPT, SKPKEY,
	SKPNKEY, MOVDLY, WTKEY, SETDLY, SETSND,
	IADD, ISETSPT, IBCD, REGDMP, REGLD,
	CHIP8_ISTR_SET_SIZE
} chip8_opcode;

typedef void chip8_istr(chip8_vm[const static 1]);

extern chip8_istr* chip8_istr_set[CHIP8_ISTR_SET_SIZE];

chip8_opcode chip8_disassemble(const chip8_word);

#endif
