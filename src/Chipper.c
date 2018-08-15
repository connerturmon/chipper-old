#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <SDL/SDL.h>

#include "Chipper.h"

// Different sprites used by the program. This resides in interpreter memory (0x50-0xA0).
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
    // Start by initializing our interpreter for use.
    Chip8 chipper;
    ChipperInitialize(rom, &chipper);

    // Initialize SDL for graphics and user input.
    uint8_t keys;
    SDL_Event event;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetVideoMode(DISPLAY_W, DISPLAY_H, DISPLAY_BPP, SDL_HWSURFACE | SDL_DOUBLEBUF);
    ChipperPrintROM(&chipper);
    // Enter main loop of the program.
    for (;;)
    {
        if (SDL_PollEvent(&event))
            if (event.key.state == SDL_PRESSED && event.key.keysym.sym == SDLK_ESCAPE)
                exit(1);
        
        // ChipperExecute(&chipper);
        ChipperDraw(&chipper);
    }
}

void ChipperInitialize(const char *rom, Chip8 *chipper)
{
    // Zero out Memory so that there is no 'noise.'
    memset(chipper->memory, 0, MEMORY_SIZE);
    memset(chipper->stack,  0, STACK_SIZE);

    // Load ROM into Memory so that we have instructions to execute.
    ChipperLoadROMToMemory(rom, chipper);

    // Load Fontset into Memory for program use.
    for (int i = 0; i < FONTSET_SIZE; i++)
        chipper->memory[FONTSET_OFFSET + i] = chip8_fontset[i];

    // Initialize Registers so that they don't have any unexpected values.
    memset(chipper->registers.V, 0, sizeof(chipper->registers.V));
    chipper->registers.DT = 0;
    chipper->registers.ST = 0;
    chipper->registers.SP = 0;
    chipper->registers.I  = 0;
    chipper->registers.PC = WORKRAM_OFFSET;   // Point to first instruction of ROM.

    memset(chipper->graphics, 0, sizeof(chipper->graphics));
    chipper->key = 0;
}

void ChipperLoadROMToMemory(const char *rom, Chip8 *chipper)
{
    FILE *file = fopen(rom, "r");

    // Make sure that the file was able to open correctly.
    if (file == NULL)
        printf("ERROR OPENING FILE\n");

    // Read the binary of the ROM into our memory (0x200 offset) so that we can execute instructions.
    fread(chipper->memory + WORKRAM_OFFSET, 1, MEMORY_SIZE - WORKRAM_OFFSET, file);
    fclose(file);
}

void ChipperDraw(Chip8 *chipper)
{ 
    int i, j;
    SDL_Surface *surface = SDL_GetVideoSurface();
    SDL_LockSurface(surface);  
    uint32_t *screen = (uint32_t *)surface->pixels;  
    memset (screen,0,surface->w*surface->h*sizeof(Uint32));
 
    // Think this has something to do with upscaling
    for (i = 0; i < DISPLAY_H; i++)
        for (j = 0; j < DISPLAY_W; j++){
            screen[j+i*surface->w] = chipper->graphics[(j/10)+(i/10)*64] ? 0xFFFFFFFF : 0;
        }
 
    SDL_UnlockSurface(surface);
    SDL_Flip(surface);
    SDL_Delay(15);
}

// Just for testing purposes...
void ChipperPrintROM(Chip8 *chipper)
{
    for (int i = 0; i < WORKRAM_OFFSET; i++)
        printf("%02X ", chipper->memory[WORKRAM_OFFSET + i]);
}