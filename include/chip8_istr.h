#ifndef CHIP8_ISTR_H
#define CHIP8_ISTR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

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
} chip8_opcode ;

typedef void chip8_instruction(chip8_vm[static 1]);

extern chip8_instruction* chip8_instruction_set[];

chip8_opcode chip8_disassemble(chip8_word);

#endif
