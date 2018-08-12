#include <stdint.h>
#include <stdio.h>
#include "Instructions.h"
#include "Interpreter.h"
#include "Memory.h"

/* Gets the registers specified in an opcode and return them in a RESULTREGISTERS struct */
const RESULTREGISTERS GetRegistersFromOpcode(const uint16_t opcode)
{
    RESULTREGISTERS result;
    result.x = (opcode & 0x0F00) >> 8;
    result.y = (opcode & 0x00F0) >> 4;

    return result;
}

const uint16_t GetAddressFromOpcode(const uint16_t opcode) { return opcode & 0x0FFF; }

/* Jump to a machine code routine at nnn */
void _SYS_addr(const uint16_t opcode, struct Registers *regs)
{
    const uint16_t address = GetAddressFromOpcode(opcode);
    regs->PC = address;
}

/* Clear the screen */
void _CLS()
{
    /* Clear screen in SDL */
}

/* Return from subroutine */
void _RET(struct Registers *regs, uint16_t stack[])
{
    regs->PC = stack[regs->SP]; /* Set PC to address at top of stack */
    regs->SP--;
}

/* Set program counter to specified address */
void _JP_addr(const uint16_t opcode, struct Registers* regs)
{
    const uint16_t address = GetAddressFromOpcode(opcode);
    regs->PC = address;
}

/* Call subroutine at specified address */
void _CALL_addr(const uint16_t opcode, struct Registers *regs, uint16_t stack[])
{
    const uint16_t address = GetAddressFromOpcode(opcode);

    printf("Address: %03X\nPC: %X\n", address, regs->PC);
    stack[regs->SP] = regs->PC;
    regs->SP++;
    regs->PC = address;
    printf("PC: %X\n", regs->PC);
}

/* Skip next instruction if Vx == specified value */
void _SE_Vx_B(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    const uint8_t val = opcode & 0x00FF;

    if (regs->V[reg] == val)
        regs->PC += 2;
}

/* Skip next instruction if Vx != specified value */
void _SNE_Vx_B(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    const uint8_t val = opcode & 0x00FF;

    if (regs->V[reg] != val)
        regs->PC += 2;
}

/* Skip next instruction if Vx == Vy */
void _SE_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);

    if (regs->V[reg.x] == regs->V[reg.y])
        regs->PC += 2;
}

/* Set Vx equal to specified value */
void _LD_Vx_B(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    const uint8_t val = opcode & 0x00FF;
    
    regs->V[reg] = val;
}

/* Adds specified value to Vx */
void _ADD_Vx_B(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    const uint8_t val = opcode & 0x00FF;

    regs->V[reg] += val;
}

/* Store the value of Vy into Vx */
void _LD_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);

    regs->V[reg.x] = regs->V[reg.y];
}

/* Vx |= Vy */
void _OR_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);
    
    regs->V[reg.x] = (regs->V[reg.x] | regs->V[reg.y]);
}

/* Vx &= Vy */
void _AND_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);

    regs->V[reg.x] = regs->V[reg.x] & regs->V[reg.y];
}

/* Vx ^= Vy */
void _XOR_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);

    regs->V[reg.x] = regs->V[reg.x] ^ regs->V[reg.y];
}

/* Vx += Vy, set VF = carry */
void _ADD_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);
    /* Check for overflow (can't do it regularly cause uint8_t wraps) */
    const uint16_t x_plus_y = regs->V[reg.x] + regs->V[reg.y];

    if (x_plus_y > 0xFF)
        regs->VF = 1;
    regs->V[reg.x] += regs->V[reg.y];
}

/* Vx -= Vy, set VF = NOT Borrow */
void _SUB_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);

    if (regs->V[reg.x] > regs->V[reg.y])
        regs->VF = 1;
    regs->V[reg.x] -= regs->V[reg.y];
}

/* Vx >> 1, set VF if least significant bit of Vx is 1 */
void _SHR_Vx(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;

    if ((regs->V[reg] & 0x01) == 1)
        regs->VF = 1;
    regs->V[reg] >>= 1;
}

/* Set Vx = Vy - Vx, set VF = NOT Borrow */
void _SUBN_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    const RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);

    if (regs->V[reg.y] > regs->V[reg.x])
        regs->VF = 1;
    regs->V[reg.x] = regs->V[reg.y] - regs->V[reg.x];
}

/* Vx << 1, set VF if most significant bit of Vx is 1 */
void _SHL_Vx(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;

    if ((regs->V[reg] & 0x80) >> 7 == 1)
        regs->VF = 1;
    regs->V[reg] <<= 1;
}

/* Skip next instruction if Vx != Vy */
void _SNE_Vx_Vy(const uint16_t opcode, struct Registers *regs)
{
    RESULTREGISTERS reg = GetRegistersFromOpcode(opcode);

    if (regs->V[reg.x] != regs->V[reg.y])
        regs->PC += 2;
}

/* Store specified address into register I */
void _LD_I_addr(const uint16_t opcode, struct Registers *regs)
{
    const uint16_t address = opcode & 0x0FFF;
    regs->I = address;
}

/* Jump to location specified + V0 */
void _JP_V0_addr(const uint16_t opcode, struct Registers *regs)
{
    const uint16_t address = (opcode & 0x0FFF) + regs->V[0];
    regs->PC = address;
}

/* Set Vx = random Byte & specified value */
void _RND_Vx_B() { printf("Feelsbadman\n"); }

/* Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision */
void _DRW_Vx_Vy_nibble() { printf("Feelsbadman\n"); }

void _SKP_Vx() { printf("Feelsbadman\n"); }
void _SKNP_Vx() { printf("Feelsbadman\n"); }

/* Store value of DT into Vx */
void _LD_Vx_DT(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    regs->V[reg] = regs->DT;
}

void _LD_Vx_K() { printf("Feelsbadman\n"); }

/* Set DT = Vx */
void _LD_DT_Vx(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    regs->DT = regs->V[reg];
}

/* Load Sound Timer into Vx */
void _LD_ST_Vx(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    regs->ST = regs->V[reg];
}

/* I = I + Vx */
void _ADD_I_Vx(const uint16_t opcode, struct Registers *regs)
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    regs->I += regs->V[reg];
}

void _LD_F_Vx() { printf("Feelsbadman\n"); }
void _LD_B_Vx() { printf("Feelsbadman\n"); }

/* Store registers V0 through Vx in memory starting at location I */
void _LD_addrI_Vx(const uint16_t opcode, struct Registers *regs, uint8_t memory[])
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= reg; i++)
        WriteByte(memory, regs->I + i, regs->V[i]);
}

/* Read registers V0 through Vx from memory starting at location I */
void _LD_Vx_addrI(const uint16_t opcode, struct Registers *regs, uint8_t memory[])
{
    const uint8_t reg = (opcode & 0x0F00) >> 8;
    for (int i = 0; i <= reg; i++)
        regs->V[i] = ReadByte(memory, regs->I + i);
}