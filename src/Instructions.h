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

void SYS_addr();

#endif /* _INSTRUCTIONS_H */