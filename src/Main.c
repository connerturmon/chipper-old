#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define MEMORY_SIZE  4096
#define STACK_SIZE   16
#define GENERAL_REGS 16
#define WINDOW_W     640
#define WINDOW_H     320
#define EMU_W        64
#define EMU_H        32

typedef uint8_t bool;

#define TRUE  1
#define FALSE 0

/* CHIP-8 Fontset utilized by programs and stored in low memeory
   (0x000-0x200). */
Uint8 fontset[80] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, /* 0 */
    0x20, 0x60, 0x20, 0x20, 0x70, /* 1 */
    0xF0, 0x10, 0xF0, 0x80, 0xF0, /* 2 */
    0xF0, 0x10, 0xF0, 0x10, 0xF0, /* 3 */
    0x90, 0x90, 0xF0, 0x10, 0x10, /* 4 */
    0xF0, 0x80, 0xF0, 0x10, 0xF0, /* 5 */
    0xF0, 0x80, 0xF0, 0x90, 0xF0, /* 6 */
    0xF0, 0x10, 0x20, 0x40, 0x40, /* 7 */
    0xF0, 0x90, 0xF0, 0x90, 0xF0, /* 8 */
    0xF0, 0x90, 0xF0, 0x10, 0xF0, /* 9 */
    0xF0, 0x90, 0xF0, 0x90, 0x90, /* A */
    0xE0, 0x90, 0xE0, 0x90, 0xE0, /* B */
    0xF0, 0x80, 0x80, 0x80, 0xF0, /* C */
    0xE0, 0x90, 0x90, 0x90, 0xE0, /* D */
    0xF0, 0x80, 0xF0, 0x80, 0xF0, /* E */
    0xF0, 0x80, 0xF0, 0x80, 0x80  /* F */
};

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
    Uint8  graphics[EMU_W * EMU_H];
} CHIP8;

void LoadROM(CHIP8 *chipper, const char *file);
void Initialize(CHIP8 *chipper);

int main(int argc, char *argv[])
{
    /* Initialize SDL. */
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        printf("ERROR INITIALIZING SDL2: %s\n", SDL_GetError());
        exit(1);
    }

    /* Create our main window and check if it intiialized properly. */
    SDL_Window *chipper_window = SDL_CreateWindow(
        "Chipper - A CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        EMU_W,
        EMU_H,
        SDL_WINDOW_SHOWN
    );
    if (chipper_window == NULL)
    {
        printf("ERROR INITIALIZING MAIN WINDOW: %s\n", SDL_GetError());
        exit(2);
    }

    /* Create our renderer. */
    SDL_Renderer *renderer = SDL_CreateRenderer(
        chipper_window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (renderer == NULL)
    {
        printf("ERROR INITIALIZING RENDERER: %s\n", SDL_GetError());
        exit(3);
    }

    /* Create our CHIP-8 and Initialize it for use. */
    CHIP8 chipper;
    Initialize(&chipper);
    LoadROM(&chipper, argv[1]);

    SDL_Event events;
    bool      chipper_running = TRUE;

    /* Enter main loop. */
    while (chipper_running)
    {
        while (SDL_PollEvent(&events))
        {
            if (events.type == SDL_QUIT)
                chipper_running = FALSE;
        }
    }

    /* Free our SDL Memory. */
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(chipper_window);
    SDL_Quit();

    return 0;
}

/* Initialize all of the CHIP8 values. */
void Initialize(CHIP8 *chipper)
{
    /* Clear out registers and initialize Program Counter. */
    memset(chipper->V, 0, sizeof(chipper->V));
    chipper->ST = 0;
    chipper->DT = 0;
    chipper->I  = NULL;
    chipper->SP = 0;
    chipper->PC = 0x200;

    chipper->opcode = 0;
    
    /* Clear all of the CHIP8 memory. */
    memset(chipper->memory,  0, sizeof(chipper->memory));
    memset(chipper->stack,   0, sizeof(chipper->stack));
    memset(chipper->graphics, 0, sizeof(chipper->graphics));
}

/* Loads desired ROM passed through command line into CHIP8 Memory. */
void LoadROM(CHIP8 *chipper, const char *file)
{
    /* Open our ROM from command line argument. */
    FILE *rom = fopen(file, "r");

    /* Get the size of the ROM so we can read it into our memeory space. */
    fseek(rom, 0, SEEK_END);
    const Uint32 rom_size = ftell(rom);
    fseek(rom, 0, SEEK_SET);

    /* Read the ROM into CHIP8 Memory space. */
    fread(chipper->memory + 0x200, 1, rom_size, rom);
}