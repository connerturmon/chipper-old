#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

#include "Chipper.h"

uint8_t chip8_fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void ChipperStart(const char *rom)
{
    ChipperInitialize(rom);
}

void ChipperInitialize(const char *rom)
{
    Chip8 chipper;

    // Zero out Memory so that there is no 'noise'
    memset(chipper.memory, 0, MEMORY_SIZE);
    memset(chipper.stack,  0, STACK_SIZE);

    // Load ROM into Memory so that we have instructions to execute
    ChipperLoadROMToMemory(rom, &chipper);

    // Load Fontset into Memory for program use
    for (int i = 0; i < FONTSET_SIZE; i++)
        chipper.memory[FONTSET_OFFSET + i] = chip8_fontset[i];

    // Initialize Registers so that they don't have any unexpected values
    memset(chipper.registers.V, 0, sizeof(chipper.registers.V));
    chipper.registers.DT = 0;
    chipper.registers.ST = 0;
    chipper.registers.SP = 0;
    chipper.registers.I  = 0;
    chipper.registers.PC = WORKRAM_OFFSET;   // Point to first instruction of ROM

    for (int i = 0; i < WORKRAM_OFFSET; i++)
        printf("%02X ", chipper.memory[WORKRAM_OFFSET + i]);
}

void ChipperLoadROMToMemory(const char *rom, Chip8 *chipper)
{
    uint16_t rom_size;
    
    FILE *file = fopen(rom, "r");

    if (file == NULL)
        printf("ERROR OPENING FILE\n");

    fread(chipper->memory + WORKRAM_OFFSET, 1, MEMORY_SIZE - WORKRAM_OFFSET, file);

    fclose(file);
}