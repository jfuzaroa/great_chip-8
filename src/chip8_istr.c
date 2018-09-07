#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/*
 * @brief Call RCA 1802 program at address NNN (Not required for most ROMs)
 */
void chip8_RCA(chip8_vm chip8[static 1])
{
    fputs("great-chip-8: RCA opcode executed.\n This shouldn't happen\n",
          stderr);
}

/*
 * @brief Clear the screen
 */
void chip8_CLS(chip8_vm chip8[static 1]);

/*
 * @brief Pops stack to return from subroutine
 */
void chip8_RET(chip8_vm chip8[static 1])
{
    chip8->sp -= 2;
    chip8->pc = chip8->mem[chip8->sp] << 8;
    chip8->pc += chip8->mem[chip8->sp+1];
}

/*
 * @brief Jumps to address at NNN
 */
void chip8_JMP(chip8_vm chip8[static 1])
{
    chip8->pc = chip8->istr_word & 0x0FFF;
}

/*
 * @brief Pushes program counter to stack and calls subroutine at NNN
 */
void chip8_CALL(chip8_vm chip8[static 1])
{
    chip8->mem[chip8->sp] = ((chip8->pc+2) & 0xFF00) >> 8;
    chip8->mem[chip8->sp+1] = (chip8->pc+2) & 0x00FF;
    chip8->sp += 2;
    chip8->pc = chip8->istr_word & 0x0FFF;
}

/*
 * @brief Skip next instruction if register V[X] equals NN
 */
void chip8_SKPEI(chip8_vm chip8[static 1])
{
    chip8_register chip8_reg = (chip8->istr_word & 0x0F00) >> 8;

    if (chip8->regs[chip8_reg] == chip8->istr_word & 0x00FF) {
        chip8->pc += 2;
    }
    chip8->pc += 2;
}

/*
 * @brief Skip next instruction if register V[X] does not equal NN
 */
void chip8_SKPNEI(chip8_vm chip8[static 1])
{
    chip8_register chip8_reg = (chip8->istr_word & 0x0F00) >> 8;

    if (chip8->regs[chip8_reg] != chip8->istr_word & 0x00FF) {
        chip8->pc += 2;
    }
    chip8->pc += 2;
}

/*
 * @brief Skip next instruction if register V[X] equals V[Y]
 */
void chip8_SKPE(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    if (chip8->regs[chip8_regx] == chip8->regs[chip8_regy]) {
        chip8->pc += 2;
    }
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to NN
 */
void chip8_MOVI(chip8_vm chip8[static 1])
{
    chip8_byte chip8_imdt = (chip8->istr_word & 0x00FF);
    chip8_register chip8_reg = (chip8->istr_word & 0x0F00) >> 8;

    chip8->regs[chip8_reg] = chip8_imdt;
    chip8->pc += 2;
}

/*
 * @brief Add NN to V[X]
 */
void chip8_ADDI(chip8_vm chip8[static 1])
{
    chip8_byte chip8_imdt = (chip8->istr_word & 0x00FF);
    chip8_register chip8_reg = (chip8->istr_word & 0x0F00) >> 8;

    chip8->regs[chip8_reg] += chip8_imdt;
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to value of V[Y]
 */
void chip8_MOV(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    chip8->regs[chip8_regx] = chip8->regs[chip8_regy];
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to V[X] OR V[Y]
 */
void chip8_OR(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    chip8->regs[chip8_regx] |= chip8->regs[chip8_regy];
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to V[X] AND V[Y]
 */
void chip8_AND(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    chip8->regs[chip8_regx] &= chip8->regs[chip8_regy];
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to V[X] XOR V[Y]
 */
void chip8_XOR(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    chip8->regs[chip8_regx] ^= chip8->regs[chip8_regy];
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to V[X] + V[Y] and set V[F] to 0 or to 1 if a carry occurs
 */
void chip8_ADD(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    if ((unsigned short) chip8->regs[chip8_regx] + chip8->regs[chip8_regy] < 0xFF) {
        chip8->regs[VF] = 0;
    } else {
        chip8->regs[VF] = 1;
    }
    chip8->regs[chip8_regx] += chip8->regs[chip8_regy];
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to V[X] - V[Y] and set V[F] to 1 or to 0 if borrow occurs
 */
void chip8_SUB(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    if (chip8->regs[chip8_regx] < chip8->regs[chip8_regy]) {
        chip8->regs[VF] = 0;
    } else {
        chip8->regs[VF] = 1;
    }
    chip8->regs[chip8_regx] -= chip8->regs[chip8_regy];
    chip8->pc += 2;
}

/*
 * @brief Set V[F] to the LSB of V[X] and shift V[X] to the right by 1
 */
void chip8_SHFR(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;

    chip8->regs[VF] = chip8->regs[chip8_regx] & 0x01;
    chip8->regs[chip8_regx] >>= 1;
    chip8->pc += 2;
}

/*
 * @brief Set V[X] to V[Y] - V[X] and set V[F] to 1 or to 0 if borrow occurs
 */
void chip8_SUBB(chip8_vm chip8[static 1]);
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    if (chip8->regs[chip8_regy] < chip8->regs[chip8_regx]) {
        chip8->regs[VF] = 0;
    } else {
        chip8->regs[VF] = 1;
    }
    chip8->regs[chip8_regx] = chip8->regs[chip8_regy] - chip8->regs[chip8_regx]
    chip8->pc += 2;
}

/*
 * @brief Set V[F] to the MSB of V[X] and shift V[X] to the left by 1
 */
void chip8_SHFL(chip8_vm chip8[static 1]);
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;

    chip8->regs[VF] = chip8->regs[chip8_regx] & 0x80;
    chip8->regs[chip8_regx] <<= 1;
    chip8->pc += 2;
}

/*
 * @brief Skip next instruction if V[X] does not equal V[Y]
 */
void chip8_SKPNE(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8_register chip8_regy = (chip8->istr_word & 0x00F0) >> 4;

    if (chip8->regs[chip8_regx] != chip8->regs[chip8_regy]) {
        chip8->pc += 2;
    }
    chip8->pc += 2;
}

/*
 * @brief Sets I to the address NNN
 */
void chip8_MIV(chip8_vm chip8[static 1])
{
    chip8->idx = chip8->istr_word & 0x0FFF;
    chip8->pc += 2;
}

/*
 * @brief Jumps to address NNN plus V[0]
 */
void chip8_JMPO(chip8_vm chip8[static 1])
{
    chip8->pc = (chip8->istr_word & 0x0FFF) + chip8->regs[V0];
}

/*
 * @brief Sets V[X] equal to a bitwise AND between a random number and NN
 */
void chip8_RNDMSK(chip8_vm chip8[static 1])
{
    chip8_register chip8_regx = (chip8->istr_word & 0x0F00) >> 8;
    chip8->regs[chip8_regx] = (rand() % 256) & (chip8->istr_word & 0x00FF);
}

// TODO
/*
 * @brief Draws a sprite at coordinate (V[X], V[Y]) with width 8xN
 */
void chip8_DRWSPT(chip8_vm chip8[static 1])
void chip8_SKPKEY(chip8_vm chip8[static 1]);
void chip8_SKPNKEY(chip8_vm chip8[static 1]);
void chip8_MOVDLY(chip8_vm chip8[static 1]);
void chip8_WTKEY(chip8_vm chip8[static 1]);
void chip8_SETDLY(chip8_vm chip8[static 1]);
void chip8_SETSND(chip8_vm chip8[static 1]);
void chip8_IADD(chip8_vm chip8[static 1]);
void chip8_ISETSPT(chip8_vm chip8[static 1]);
void chip8_IBCD(chip8_vm chip8[static 1]);
void chip8_REGDMP(chip8_vm chip8[static 1]);
void chip8_REGLD(chip8_vm chip8[static 1]);
