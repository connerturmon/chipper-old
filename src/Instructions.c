#include <stdint.h>
#include "Instructions.h"
#include "Interpreter.h"

/* Jump to a machine code routine at nnn */
void _SYS_addr(const uint16_t opcode, struct registers *regs)
{
    uint16_t address = GetAddressFromOpcode(opcode);
    regs->PC = address;
}

/* Clear the screen */
void _CLS()
{
    /* Clear screen in SDL */
}

/* Return from subroutine */
void _RET(struct registers *regs, uint8_t stack[])
{
    regs->PC = stack[regs->SP]; /* Set PC to address at top of stack */
    regs->SP--;
}

/* Set program counter to specified address */
void _JP_addr(const uint16_t opcode, struct registers* regs)
{
    uint16_t address = GetAddressFromOpcode(opcode);
    regs->PC = address;
}

/* Call subroutine at specified address */
void _CALL_addr(const uint16_t opcode, struct registers *regs, uint16_t stack[])
{
    uint16_t address = GetAddressFromOpcode(opcode);

    regs->SP++;
    stack[regs->SP] = regs->PC;
    regs->PC = address; 
}

/* Skip next instruction if Vx == specified value */
void _SE_Vx_B(uint16_t opcode, struct registers *regs)
{
    uint16_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;

    if (regs->V[reg] == val)
        regs->PC += 2;
}

/* Skip next instruction if Vx != specified value */
void _SNE_Vx_B(uint16_t opcode, struct registers *regs)
{
    uint8_t reg = (opcode & 0x0F00) >> 8;
    uint8_t val = opcode & 0x00FF;
}