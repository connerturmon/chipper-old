#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <stdint.h>

typedef struct Registers
{
    uint8_t  V[15]; /* 15 General purpose registers V0-VE */
    uint8_t  VF;    /* Flag register */
    uint8_t  SP;    /* Stack pointer points to top of stack */
    uint16_t PC;    /* Program counter keeps track of current instruction */
    uint16_t I;     /* Used to store memory addresses for operations */

    /* Timers */
    uint8_t  DT;    /* Delay timer counts decrements from non-zero value at 60Hz rate */
    uint8_t  ST;    /* Sound timer "    " and beeps upon 0 */
} Registers;

void InitializeRegisters(struct Registers *regs);

#endif /* _INTERPRETER_H */
