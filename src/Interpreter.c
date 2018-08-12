#include "Interpreter.h"

/* Zero out all of the registers */
void InitializeRegisters(struct Registers *regs)
{
    for (int i = 0; i <= 15; i++)
        regs->V[i] = 0;
    regs->VF = 0;
    regs->SP = 0;
    regs->ST = 0;
    regs->PC = 0;
    regs->I  = 0;
    regs->DT = 0;
}