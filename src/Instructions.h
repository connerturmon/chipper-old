/**
 * Instructions.h - Declaration of Chip-8 operations.
 * 
 * This header file includes declaration of the Chip-8 Instructions that
 * are used by the interpreter. See 'Instructions.c' for the definitions
 * of said functions.
 * 
 * Chip-8 uses 2-Byte instructions with the format 0xIPPP where the
 * instruction number (I) is follwed by parameters (PPP). Sometimes the
 * last P can be a instruction specifier (i.e., IPPI).
 */

#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include <stdint.h>
#include "Interpreter.h"

typedef struct RESULTREGISTERS
{
    uint8_t x;
    uint8_t y;
} RESULTREGISTERS;

const uint16_t GetAddressFromOpcode(const uint16_t opcode) { return opcode & 0x0FFF; }
const RESULTREGISTERS GetRegistersFromOpcode(const uint16_t opcode);

void _SYS_addr(
    const uint16_t opcode,
    struct registers *regs
);
void _CLS();
void _RET(
    struct registers *regs,
    uint8_t stack[]
);
void _JP_addr(
    const uint16_t opcode,
    struct registers *regs
);
void _CALL_addr(
    const uint16_t opcode,
    struct registers *regs,
    uint16_t stack[]
);
void _SE_Vx_B(
    const uint16_t opcode,
    struct registers *regs
);
void _SNE_Vx_B(
    const uint16_t opcode,
    struct registers *regs
);
void _SE_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _LD_Vx_B(
    const uint16_t opcode,
    struct registers *regs
);
void _ADD_Vx_B(
    const uint16_t opcode,
    struct registers *regs
);
void _LD_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _OR_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _AND_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _XOR_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _ADD_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _SUB_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _SHR_Vx(
    const uint16_t opcode,
    struct registers *regs
);
void _SUBN_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _SHL_Vx(
    const uint16_t opcode,
    struct registers *regs
);
void _SNE_Vx_Vy(
    const uint16_t opcode,
    struct registers *regs
);
void _LD_I_addr(
    const uint16_t opcode,
    struct registers *regs
);
void _JP_V0_addr(
    const uint16_t opcode,
    struct registers *regs
);
void _RND_Vx_B();
void _DRW_Vx_Vy_nibble();
void _SKP_Vx();
void _SKNP_Vx();
void _LD_Vx_DT(
    const uint16_t opcode,
    struct registers *regs
);
void _LD_Vx_K();
void _LD_DT_Vx(
    const uint16_t opcode,
    struct registers *regs
);
void _LD_ST_Vx(
    const uint16_t opcode,
    struct registers *regs
);
void _ADD_I_Vx(
    const uint16_t opcode,
    struct registers *regs
);
void _LD_F_Vx();
void _LD_B_Vx();
void _LD_addrI_Vx(
    const uint16_t opcode,
    struct registers *regs,
    uint8_t memory[]
);
void _LD_Vx_addrI(
    const uint16_t opcode,
    struct registers *regs,
    uint8_t memory[]
);

#endif /* _INSTRUCTIONS_H */