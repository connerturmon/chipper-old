#ifndef _MEMORY_H
#define _MEMORY_H

#include <stdint.h>

uint8_t ReadByte(uint8_t memory[], uint16_t address);
uint8_t WriteByte(uint8_t memory[], uint16_t address);

#endif /* _MEMORY_H */