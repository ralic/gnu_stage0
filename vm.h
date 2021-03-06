/* Copyright (C) 2016 Jeremiah Orians
 * This file is part of stage0.
 *
 * stage0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * stage0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with stage0.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Virtual machine state */
struct lilith
{
	uint8_t *memory;
	size_t amount_of_Ram;
	uint32_t reg[16];
	uint32_t ip;
	bool halted;
	bool exception;
};

/* Unpacked instruction */
struct Instruction
{
	uint32_t ip;
	uint8_t raw0, raw1, raw2, raw3;
	char opcode[3];
	uint32_t raw_XOP;
	char XOP[6];
	char operation[9];
	int16_t raw_Immediate;
	char Immediate[7];
	uint32_t HAL_CODE;
	uint8_t reg0;
	uint8_t reg1;
	uint8_t reg2;
	uint8_t reg3;
	bool invalid;
};

/* Prototypes for functions in vm_instructions.c*/
void vm_FOPEN_READ(struct lilith* vm);
void vm_FOPEN_WRITE(struct lilith* vm);
void vm_FCLOSE(struct lilith* vm);
void vm_FSEEK(struct lilith* vm);
void vm_REWIND(struct lilith* vm);
void vm_FGETC(struct lilith* vm);
void vm_FPUTC(struct lilith* vm);
void ADD_CI(struct lilith* vm, struct Instruction* c);
void ADD_CO(struct lilith* vm, struct Instruction* c);
void ADD_CIO(struct lilith* vm, struct Instruction* c);
void ADDU_CI(struct lilith* vm, struct Instruction* c);
void ADDU_CO(struct lilith* vm, struct Instruction* c);
void ADDU_CIO(struct lilith* vm, struct Instruction* c);
void SUB_BI(struct lilith* vm, struct Instruction* c);
void SUB_BO(struct lilith* vm, struct Instruction* c);
void SUB_BIO(struct lilith* vm, struct Instruction* c);
void SUBU_BI(struct lilith* vm, struct Instruction* c);
void SUBU_BO(struct lilith* vm, struct Instruction* c);
void SUBU_BIO(struct lilith* vm, struct Instruction* c);
void MULTIPLY(struct lilith* vm, struct Instruction* c);
void MULTIPLYU(struct lilith* vm, struct Instruction* c);
void DIVIDE(struct lilith* vm, struct Instruction* c);
void DIVIDEU(struct lilith* vm, struct Instruction* c);
void MUX(struct lilith* vm, struct Instruction* c);
void NMUX(struct lilith* vm, struct Instruction* c);
void SORT(struct lilith* vm, struct Instruction* c);
void SORTU(struct lilith* vm, struct Instruction* c);
void ADD(struct lilith* vm, struct Instruction* c);
void ADDU(struct lilith* vm, struct Instruction* c);
void SUB(struct lilith* vm, struct Instruction* c);
void SUBU(struct lilith* vm, struct Instruction* c);
void CMP(struct lilith* vm, struct Instruction* c);
void CMPU(struct lilith* vm, struct Instruction* c);
void MUL(struct lilith* vm, struct Instruction* c);
void MULH(struct lilith* vm, struct Instruction* c);
void MULU(struct lilith* vm, struct Instruction* c);
void MULUH(struct lilith* vm, struct Instruction* c);
void DIV(struct lilith* vm, struct Instruction* c);
void MOD(struct lilith* vm, struct Instruction* c);
void DIVU(struct lilith* vm, struct Instruction* c);
void MODU(struct lilith* vm, struct Instruction* c);
void MAX(struct lilith* vm, struct Instruction* c);
void MAXU(struct lilith* vm, struct Instruction* c);
void MIN(struct lilith* vm, struct Instruction* c);
void MINU(struct lilith* vm, struct Instruction* c);
void AND(struct lilith* vm, struct Instruction* c);
void OR(struct lilith* vm, struct Instruction* c);
void XOR(struct lilith* vm, struct Instruction* c);
void NAND(struct lilith* vm, struct Instruction* c);
void NOR(struct lilith* vm, struct Instruction* c);
void XNOR(struct lilith* vm, struct Instruction* c);
void MPQ(struct lilith* vm, struct Instruction* c);
void LPQ(struct lilith* vm, struct Instruction* c);
void CPQ(struct lilith* vm, struct Instruction* c);
void BPQ(struct lilith* vm, struct Instruction* c);
void SAL(struct lilith* vm, struct Instruction* c);
void SAR(struct lilith* vm, struct Instruction* c);
void SL0(struct lilith* vm, struct Instruction* c);
void SR0(struct lilith* vm, struct Instruction* c);
void SL1(struct lilith* vm, struct Instruction* c);
void SR1(struct lilith* vm, struct Instruction* c);
void ROL(struct lilith* vm, struct Instruction* c);
void ROR(struct lilith* vm, struct Instruction* c);
void LOADX(struct lilith* vm, struct Instruction* c);
void LOADX8(struct lilith* vm, struct Instruction* c);
void LOADXU8(struct lilith* vm, struct Instruction* c);
void LOADX16(struct lilith* vm, struct Instruction* c);
void LOADXU16(struct lilith* vm, struct Instruction* c);
void LOADX32(struct lilith* vm, struct Instruction* c);
void LOADXU32(struct lilith* vm, struct Instruction* c);
void STOREX(struct lilith* vm, struct Instruction* c);
void STOREX8(struct lilith* vm, struct Instruction* c);
void STOREX16(struct lilith* vm, struct Instruction* c);
void STOREX32(struct lilith* vm, struct Instruction* c);
void NEG(struct lilith* vm, struct Instruction* c);
void ABS(struct lilith* vm, struct Instruction* c);
void NABS(struct lilith* vm, struct Instruction* c);
void SWAP(struct lilith* vm, struct Instruction* c);
void COPY(struct lilith* vm, struct Instruction* c);
void MOVE(struct lilith* vm, struct Instruction* c);
void BRANCH(struct lilith* vm, struct Instruction* c);
void CALL(struct lilith* vm, struct Instruction* c);
void READPC(struct lilith* vm, struct Instruction* c);
void READSCID(struct lilith* vm, struct Instruction* c);
void FALSE(struct lilith* vm, struct Instruction* c);
void TRUE(struct lilith* vm, struct Instruction* c);
void JSR_COROUTINE(struct lilith* vm, struct Instruction* c);
void RET(struct lilith* vm, struct Instruction* c);
void PUSHPC(struct lilith* vm, struct Instruction* c);
void POPPC(struct lilith* vm, struct Instruction* c);
void ADDI(struct lilith* vm, struct Instruction* c);
void ADDUI(struct lilith* vm, struct Instruction* c);
void SUBI(struct lilith* vm, struct Instruction* c);
void SUBUI(struct lilith* vm, struct Instruction* c);
void CMPI(struct lilith* vm, struct Instruction* c);
void LOAD(struct lilith* vm, struct Instruction* c);
void LOAD8(struct lilith* vm, struct Instruction* c);
void LOADU8(struct lilith* vm, struct Instruction* c);
void LOAD16(struct lilith* vm, struct Instruction* c);
void LOADU16(struct lilith* vm, struct Instruction* c);
void LOAD32(struct lilith* vm, struct Instruction* c);
void LOADU32(struct lilith* vm, struct Instruction* c);
void CMPUI(struct lilith* vm, struct Instruction* c);
void STORE(struct lilith* vm, struct Instruction* c);
void STORE8(struct lilith* vm, struct Instruction* c);
void STORE16(struct lilith* vm, struct Instruction* c);
void STORE32(struct lilith* vm, struct Instruction* c);
void JUMP_C(struct lilith* vm, struct Instruction* c);
void JUMP_B(struct lilith* vm, struct Instruction* c);
void JUMP_O(struct lilith* vm, struct Instruction* c);
void JUMP_G(struct lilith* vm, struct Instruction* c);
void JUMP_GE(struct lilith* vm, struct Instruction* c);
void JUMP_E(struct lilith* vm, struct Instruction* c);
void JUMP_NE(struct lilith* vm, struct Instruction* c);
void JUMP_LE(struct lilith* vm, struct Instruction* c);
void JUMP_L(struct lilith* vm, struct Instruction* c);
void JUMP_Z(struct lilith* vm, struct Instruction* c);
void JUMP_NZ(struct lilith* vm, struct Instruction* c);
void CALLI(struct lilith* vm, struct Instruction* c);
void LOADI(struct lilith* vm, struct Instruction* c);
void LOADUI(struct lilith* vm, struct Instruction* c);
void SALI(struct lilith* vm, struct Instruction* c);
void SARI(struct lilith* vm, struct Instruction* c);
void SL0I(struct lilith* vm, struct Instruction* c);
void SR0I(struct lilith* vm, struct Instruction* c);
void SL1I(struct lilith* vm, struct Instruction* c);
void SR1I(struct lilith* vm, struct Instruction* c);
void LOADR(struct lilith* vm, struct Instruction* c);
void LOADR8(struct lilith* vm, struct Instruction* c);
void LOADRU8(struct lilith* vm, struct Instruction* c);
void LOADR16(struct lilith* vm, struct Instruction* c);
void LOADRU16(struct lilith* vm, struct Instruction* c);
void LOADR32(struct lilith* vm, struct Instruction* c);
void LOADRU32(struct lilith* vm, struct Instruction* c);
void STORER(struct lilith* vm, struct Instruction* c);
void STORER8(struct lilith* vm, struct Instruction* c);
void STORER16(struct lilith* vm, struct Instruction* c);
void STORER32(struct lilith* vm, struct Instruction* c);
void JUMP(struct lilith* vm, struct Instruction* c);
void JUMP_P(struct lilith* vm, struct Instruction* c);
void JUMP_NP(struct lilith* vm, struct Instruction* c);
void CMPJUMPI_G(struct lilith* vm, struct Instruction* c);
void CMPJUMPI_GE(struct lilith* vm, struct Instruction* c);
void CMPJUMPI_E(struct lilith* vm, struct Instruction* c);
void CMPJUMPI_NE(struct lilith* vm, struct Instruction* c);
void CMPJUMPI_LE(struct lilith* vm, struct Instruction* c);
void CMPJUMPI_L(struct lilith* vm, struct Instruction* c);
void CMPJUMPUI_G(struct lilith* vm, struct Instruction* c);
void CMPJUMPUI_GE(struct lilith* vm, struct Instruction* c);
void CMPJUMPUI_LE(struct lilith* vm, struct Instruction* c);
void CMPJUMPUI_L(struct lilith* vm, struct Instruction* c);
void CMPSKIPI_G(struct lilith* vm, struct Instruction* c);
void CMPSKIPI_GE(struct lilith* vm, struct Instruction* c);
void CMPSKIPI_E(struct lilith* vm, struct Instruction* c);
void CMPSKIPI_NE(struct lilith* vm, struct Instruction* c);
void CMPSKIPI_LE(struct lilith* vm, struct Instruction* c);
void CMPSKIPI_L(struct lilith* vm, struct Instruction* c);
void CMPSKIPUI_G(struct lilith* vm, struct Instruction* c);
void CMPSKIPUI_GE(struct lilith* vm, struct Instruction* c);
void CMPSKIPUI_LE(struct lilith* vm, struct Instruction* c);
void CMPSKIPUI_L(struct lilith* vm, struct Instruction* c);
void PUSHR(struct lilith* vm, struct Instruction* c);
void PUSH8(struct lilith* vm, struct Instruction* c);
void PUSH16(struct lilith* vm, struct Instruction* c);
void PUSH32(struct lilith* vm, struct Instruction* c);
void POPR(struct lilith* vm, struct Instruction* c);
void POP8(struct lilith* vm, struct Instruction* c);
void POPU8(struct lilith* vm, struct Instruction* c);
void POP16(struct lilith* vm, struct Instruction* c);
void POPU16(struct lilith* vm, struct Instruction* c);
void POP32(struct lilith* vm, struct Instruction* c);
void POPU32(struct lilith* vm, struct Instruction* c);
void ANDI(struct lilith* vm, struct Instruction* c);
void ORI(struct lilith* vm, struct Instruction* c);
void XORI(struct lilith* vm, struct Instruction* c);
void NANDI(struct lilith* vm, struct Instruction* c);
void NORI(struct lilith* vm, struct Instruction* c);
void XNORI(struct lilith* vm, struct Instruction* c);
void NOT(struct lilith* vm, struct Instruction* c);
void CMPSKIP_G(struct lilith* vm, struct Instruction* c);
void CMPSKIP_GE(struct lilith* vm, struct Instruction* c);
void CMPSKIP_E(struct lilith* vm, struct Instruction* c);
void CMPSKIP_NE(struct lilith* vm, struct Instruction* c);
void CMPSKIP_LE(struct lilith* vm, struct Instruction* c);
void CMPSKIP_L(struct lilith* vm, struct Instruction* c);
void CMPSKIPU_G(struct lilith* vm, struct Instruction* c);
void CMPSKIPU_GE(struct lilith* vm, struct Instruction* c);
void CMPSKIPU_LE(struct lilith* vm, struct Instruction* c);
void CMPSKIPU_L(struct lilith* vm, struct Instruction* c);
void CMPJUMP_G(struct lilith* vm, struct Instruction* c);
void CMPJUMP_GE(struct lilith* vm, struct Instruction* c);
void CMPJUMP_E(struct lilith* vm, struct Instruction* c);
void CMPJUMP_NE(struct lilith* vm, struct Instruction* c);
void CMPJUMP_LE(struct lilith* vm, struct Instruction* c);
void CMPJUMP_L(struct lilith* vm, struct Instruction* c);
void CMPJUMPU_G(struct lilith* vm, struct Instruction* c);
void CMPJUMPU_GE(struct lilith* vm, struct Instruction* c);
void CMPJUMPU_LE(struct lilith* vm, struct Instruction* c);
void CMPJUMPU_L(struct lilith* vm, struct Instruction* c);

/* Prototypes for functions in vm_decode.c*/
struct lilith* create_vm(size_t size);
void destroy_vm(struct lilith* vm);
void read_instruction(struct lilith* vm, struct Instruction *current);
void eval_instruction(struct lilith* vm, struct Instruction* current);
void outside_of_world(struct lilith* vm, uint32_t place, char* message);

/* Allow tape names to be effectively changed */
char* tape_01_name;
char* tape_02_name;
