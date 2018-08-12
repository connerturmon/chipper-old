#include "Memory.h"

uint8_t ReadByte(uint8_t memory[], const uint16_t address)
    { return memory[address]; }

void WriteByte(uint8_t memory[], const uint16_t address, const uint8_t value)
    { memory[address] = value; }

void InitializeMemory(uint8_t memory[], uint16_t stack[])
{
    const unsigned short memory_size = 4096;
    const unsigned short stack_size  = 16;

    for (int i = 0; i < memory_size; i++)
        memory[i] = 0;
    for (int i = 0; i < stack_size; i++)
        stack[i] = 0;
}