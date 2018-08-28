#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define MEMORY_SIZE  4096
#define STACK_SIZE   16
#define GENERAL_REGS 16
#define WINDOW_W     640
#define WINDOW_H     320
#define GRAPHICS_W   64
#define GRAPHICS_H   32

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
    Uint32 graphics[GRAPHICS_W * GRAPHICS_H];
} CHIP8;

void LoadROM(CHIP8 *chipper, const char *file);
void Initialize(CHIP8 *chipper);
void Execute(CHIP8 *chipper);
void LoadFontset(CHIP8 *chipper);

int main(int argc, char *argv[])
{
    /* First things first, check to make sure a ROM was supplied to the program. */
    if (argc < 2)
    {
        printf("Error Opening ROM: Make sure you pass a ROM file as a parameter.\n");
        exit(20);
    }

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
        WINDOW_W,
        WINDOW_H,
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

    SDL_Texture *display = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        GRAPHICS_W,
        GRAPHICS_H
    );
    if (display == NULL)
    {
        printf("ERROR CREATING DISPLAY: %s\n", SDL_GetError());
        exit(4);
    }

    /* Enter main loop. */
    while (chipper_running)
    {
        while (SDL_PollEvent(&events))
        {
            if (events.type == SDL_QUIT)
                chipper_running = FALSE;
        }
        
        Execute(&chipper);
        SDL_UpdateTexture(display, NULL, chipper.graphics, (64 * sizeof(Uint32)));
        SDL_RenderCopy(renderer, display, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    /* Free our SDL Memory. */
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(chipper_window);
    SDL_DestroyTexture(display);
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
    chipper->I  = 0;
    chipper->SP = 0;
    chipper->PC = 0x200;

    chipper->opcode = 0;
    
    /* Clear all of the CHIP8 memory. */
    memset(chipper->memory,   0, sizeof(chipper->memory));
    memset(chipper->stack,    0, sizeof(chipper->stack));
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

void Execute(CHIP8 *chipper)
{
    Uint32 overflow_check;
    Uint8  x, y, h, pixel;

    for (int i = 0; i < 12; i++)
    {
        chipper->opcode = (chipper->memory[chipper->PC] << 8) | chipper->memory[chipper->PC + 1];

        printf("Executing opcode: %04X at %u | SP: %u | I: %u\n", chipper->opcode, chipper->PC, chipper->SP, chipper->I);
        
        switch (chipper->opcode & 0xF000)
        {
        case 0x0000:
            
            switch (chipper->opcode & 0x00FF)
            {
            case 0x0000:
                chipper->PC = chipper->opcode & 0x0FFF;
            break;

            case 0x00E0:
                /* Set all pixels to black. */
                memset(chipper->graphics, 0xFF000000, sizeof(chipper->graphics));
                chipper->PC += 2;
            break;

            case 0x00EE:
                /* Set PC to top of stack, then subtract one from SP. */
                chipper->PC = chipper->stack[chipper->SP--];
            break;

            default:
                printf("Unexpected operation in 0x0 Set: %04X\n", chipper->opcode);
            break;
            }

        case 0x1000:
            chipper->PC = (chipper->opcode & 0x0FFF);
        break;

        case 0x2000:
            chipper->SP++;
            chipper->stack[chipper->SP] = chipper->PC;
            chipper->PC = (chipper->opcode & 0x0FFF);
        break;

        case 0x3000:
            if ( (chipper->opcode & 0x00FF) == chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] )
                chipper->PC += 4;
            else
                chipper->PC += 2;
        break;

        case 0x4000:
            if ( (chipper->opcode & 0x00FF) != chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] )
                chipper->PC += 4;
            else
                chipper->PC += 2;
        break;

        case 0x5000:
            if ( chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] == chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] )
                chipper->PC += 4;
            else
                chipper->PC += 2;
        break;

        case 0x6000:
            chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] = (chipper->opcode & 0x00FF);
            chipper->PC += 2;
        break;

        case 0x7000:
            chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] += (chipper->opcode & 0x00FF);
            chipper->PC += 2;
        break;

        case 0x8000:

            switch (chipper->opcode & 0x000F)
            {
            case 0x0:
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] = chipper->V[ (chipper->opcode & 0x00F0) >> 4 ];
                chipper->PC += 2;
            break;

            case 0x1:
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] =
                    chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] | chipper->V[ (chipper->opcode & 0x00F0) >> 4 ];
                chipper->PC += 2;
            break;

            case 0x2:
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] =
                    chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] & chipper->V[ (chipper->opcode & 0x00F0) >> 4 ];
                chipper->PC += 2;
            break;

            case 0x3:
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] =
                    chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] ^ chipper->V[ (chipper->opcode & 0x00F0) >> 4 ];
                chipper->PC += 2;
            break;

            case 0x4:
                overflow_check = chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] + chipper->V[ (chipper->opcode & 0x00F0) >> 4 ];
                if (overflow_check > 0xFF)
                    chipper->V[0xF] = 1;
                else
                    chipper->V[0xF] = 0;
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] += chipper->V[ (chipper->opcode & 0x00F0) >> 4 ];
                chipper->PC += 2;
            break;

            case 0x5:
                if ( chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] > chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] )
                    chipper->V[0xF] = 1;
                else
                    chipper->V[0xF] = 0;
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] -= chipper->V[ (chipper->opcode & 0x00F0) >> 4 ];
                chipper->PC += 2;
            break;

            case 0x6:
                chipper->V[0xF] = chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] & 0b00000001;
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] = (chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] >> 1);
                chipper->PC += 2;
            break;

            case 0x7:
                if ( chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] < chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] )
                    chipper->V[0xF] = 1;
                else
                    chipper->V[0xF] = 0;
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] =
                    chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] - chipper->V[ (chipper->opcode & 0x0F00) >> 8 ];
                chipper->PC += 2;
            break;

            case 0xE:
                chipper->V[0xF] = (chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] & 0b10000000) >> 7;
                chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] = (chipper->V[ (chipper->opcode & 0x00F0) >> 4 ] << 1);
                chipper->PC += 2;
            break;

            default:
                printf("Unexpected operation in 0x8 set: %04X\n", chipper->opcode);
            break;
            }

        break;

        case 0x9000:
            if (chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] != chipper->V[ (chipper->opcode & 0x00F0) >> 4 ])
                chipper->PC += 4;
            else
                chipper->PC += 2;
            break;

        case 0xA000:
            chipper->I = (chipper->opcode & 0x0FFF);
            chipper->PC += 2;
        break;

        case 0xB000:
            chipper->PC = (chipper->opcode & 0x0FFF) + chipper->V[0];
        break;

        case 0xC000:
            chipper->V[ (chipper->opcode & 0x0F00) >> 8 ] = (rand() & (chipper->opcode & 0x00FF));
            chipper->PC += 2;
        break;

        case 0xD000:

            chipper->PC += 2;
        break;

        default:
            printf("Unkown opcode %04X\n", chipper->opcode);
        break;
        }
    }
}

void LoadFontset(CHIP8 *chipper)
{
    for (int i = 0; i < sizeof(fontset); i++)
        chipper->memory[i] = fontset[i];
}