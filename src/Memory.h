#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>

typedef struct Memory
{
    uint8_t  main[4096];
    uint16_t stack[16];
} Memory;

uint8_t ReadByte(
    uint8_t memory[],
    const uint16_t address
);

void WriteByte(
    uint8_t memory[], 
    const uint16_t address, 
    const uint8_t value
);

void InitializeMemory(uint8_t memory[], uint16_t stack[]);

#endif /* _MEMORY_H */