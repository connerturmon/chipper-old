#ifndef _CHIPPER_H
#define _CHIPPER_H

#include <stdint.h>

#define MEMORY_SIZE  4096
#define STACK_SIZE   16
#define FONTSET_SIZE 80

#define FONTSET_OFFSET 0x50
#define WORKRAM_OFFSET 0x200

uint8_t chip8_fontset[FONTSET_SIZE];

// This struct defines the memory and registers for the Chip 8 interpreter
typedef struct Chip8
{
    struct registers
    {
        uint8_t  V[16];     // General purpose registers
        uint8_t  DT, ST;    // Delay and Sound Timer
        uint8_t  SP;        // Stack Pointer
        uint16_t I;         // Used to store addresses
        uint16_t PC;        // Program Counter
    } registers;

    uint8_t  memory[MEMORY_SIZE];
    uint16_t stack[STACK_SIZE];

    uint16_t opcode;

    uint8_t  key;
    uint8_t  graphics[64 * 32];
} Chip8;

void ChipperStart(const char *rom);
void ChipperInitialize(const char *rom);
void ChipperLoadROMToMemory(
    const char *rom,
    Chip8 *chipper
);

#endif /* _CHIPPER_H */