#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include <stdint.h>

typedef struct registers
{
    /* General purpose registers */
    uint8_t
        V1, V2, V3, V4, V5,
        V6, V7, V8, V9, VA,
        VB, VC, VD, VE;
    uint8_t  VF;    /* Flag register */
    uint8_t  SP;    /* Stack pointer points to top of stack */
    uint16_t PC;    /* Program counter keeps track of current instruction */
    uint16_t I;     /* Used to store memory addresses for operations */
} registers;

#endif /* _INTERPRETER_H */