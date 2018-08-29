#ifndef _CHIPPER_H
#define _CHIPPER_H

#include <SDL2/SDL.h>

#define GENERAL_REGS 16
#define MEMORY_SIZE  4096
#define GRAPHICS_W   64
#define GRAPHICS_H   32
#define WINDOW_W     640
#define WINDOW_H     320

typedef uint8_t bool;
#define TRUE  1
#define FALSE 0

/* Our CHIP-8 struct that will hold all of our registers,
   memory, graphics, etc. */
typedef struct CHIP8
{
    /* CHIP-8 Registers */
    Uint8  V[GENERAL_REGS]; /* General Purpose Registers */
    Uint8  ST;              /* Sound Timer */
    Uint8  DT;              /* Delay Timer */
    Uint8  SP;              /* Stack Pointer */
    Uint16 PC;              /* Program Counter */
    Uint16 I;               /* Holds Addresses */

    /* Current opcode */
    Uint16 opcode;

    /* CHIP8 Memory */
    Uint16 stack[16];
    Uint8  memory[MEMORY_SIZE];
    Uint32 graphics[GRAPHICS_W * GRAPHICS_H];
    Uint8  key[0x10];
} CHIP8;

/* Start our CHIP-8 system by loading our ROM and initializing everything. */
void ChipperStart(const char *rom_file);
void ChipperInitialize(CHIP8 *chipper, const char *rom_file);
void ChipperDraw(CHIP8 *chipper, SDL_Renderer *renderer);
void ChipperTimers(CHIP8 *chipper);
void ChipperExecute(CHIP8 *chipper);

#endif /* _CHIPPER_H */