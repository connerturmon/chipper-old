#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>

uint8_t ReadByte(
    uint8_t memory[],
    const uint16_t address
) { return memory[address]; }

void WriteByte(
    uint8_t memory[], 
    const uint16_t address, 
    const uint8_t value
) { memory[address] = value; }

#endif /* _MEMORY_H */