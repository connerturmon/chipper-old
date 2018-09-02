#include <stdio.h>

#include "Chipper.h"

/* CHIP-8 Fontset utilized by programs and stored in low memeory
   (0x000-0x200). */
const Uint8 fontset[80] =
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

/* Because the CHIP-8 keys are stored in memory as
   0x0 - 0xF (16 keys), we map our keyboard to 0-16. */
static Uint8 keymap[0x10] =
{
    SDL_SCANCODE_X,
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_C,
    SDL_SCANCODE_4,
    SDL_SCANCODE_R,
    SDL_SCANCODE_F,
    SDL_SCANCODE_V
};

void ChipperStart(const char *rom_file)
{
    /* Create our Chip-8! */
    CHIP8 chipper;

    /* Set up our Chip-8 system. */
    ChipperInitialize(&chipper, rom_file);

    /* Initialize SDL2 for our emulator. */
    if (SDL_Init(SDL_INIT_EVERYTHING))
    {
        printf("Error Initializing SDL2: %s", SDL_GetError());
        exit(1);
    }

    /* Create our program's main window. */
    SDL_Window *window = SDL_CreateWindow(
        "Chipper - A CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_W,
        WINDOW_H,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL)
    {
        printf("Error Creating Window: %s", SDL_GetError());
        exit(2);
    }

    /* Create our renderer which is used to draw the game's graphics.
       The graphics array in the CHIP8 structure is an array of pixels
       that the game modifies. SDL2 uses this array of pixels to create
       a texture which is then rendered to the screen. */
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );
    if (renderer == NULL)
    {
        printf("Error Creating Renderer: %s", SDL_GetError());
        exit(3);
    }

    /* Create our texture that uses RGBA format. */
    SDL_Texture *screen = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        GRAPHICS_W,
        GRAPHICS_H
    );
    if (screen == NULL)
        exit(30);

    /* Create our event so we can check for exit. */
    SDL_Event ev;
    bool      running = TRUE;

    /* Here is our main game loop. */
    while (running)
    {
        while (SDL_PollEvent(&ev) != 0)
            if (ev.type == SDL_QUIT)
                running = FALSE;
        
        ChipperExecute(&chipper);
        ChipperDraw(&chipper, renderer, screen);
    }
}

/* Initialize our CHIP-8 system. */
void ChipperInitialize(CHIP8 *chipper, const char *rom_file)
{
    /* Load our ROM into memory. Program memory starts at 0x200 on
       the CHIP-8 since the interpreter resides in the first 0x1FF
       bytes. */
    FILE *rom = fopen(rom_file, "r");
    fread(chipper->memory + 0x200, 1, sizeof(chipper->memory), rom);

    /* Load our fontset into memory. */
    for (int i = 0; i < 80; i++)
        chipper->memory[i] = fontset[i];

    /* Clear Registers, Graphics, and Stack. */
    memset(chipper->V,        0, sizeof(chipper->V));
    memset(chipper->graphics, 0, sizeof(chipper->graphics));
    memset(chipper->stack,    0, sizeof(chipper->stack));

    /* Set the Program Counter to the start of the program. */
    chipper->PC = 0x200;
    chipper->SP = 0;
}

/* Draw our screen based on our chipper graphics array. */
void ChipperDraw(CHIP8 *chipper, SDL_Renderer *renderer, SDL_Texture *screen)
{
    /* Update our texture with our graphics array. This is where the sprites
       are drawn to. */
    SDL_UpdateTexture(screen, NULL, chipper->graphics, (GRAPHICS_W * sizeof(Uint32)));
    SDL_RenderCopy(renderer, screen, NULL, NULL);

    SDL_RenderPresent(renderer);
}

/* Decrement our timers by a rate 60Hz. */
void ChipperTimers(CHIP8 *chipper)
{
    /* If our delay timer or sound timer is not 0, then decrement
       it by a rate of 60Hz. */
    if (chipper->DT > 0)
        chipper->DT--;
    if (chipper->ST > 0)
        chipper->ST--;
    /* If your sound timer is not 0, play a bell sound through the
       speaker. */
    if (chipper->ST != 0)
        printf("%c", 7);
}

/* Here's the heart of our CHIP-8 emulator. This bad boy emulates the fetch,
   decode, execute cycle. */
void ChipperExecute(CHIP8 *chipper)
{
    Uint8 *keystate;
    Uint8 x, y, h, sprite_row;

    for (int cycles = 0; cycles < 10; cycles++)
    {
        /* Get our opcode by reading the byte at the PC and shifting it right by 8, and then adding
           the next instruction to it. We do this because all CHIP-8 instructions are 2 Bytes long. */
        chipper->opcode = (chipper->memory[chipper->PC] << 8) | chipper->memory[chipper->PC + 1];
        printf("Executing %04X at %04X | I: %02X | SP: %02X\n", chipper->opcode, chipper->PC, chipper->I, chipper->SP);

        switch (chipper->opcode & 0xF000)
        {
        case 0x0000:
            switch (chipper->opcode & 0x000F)
            {
            case 0x0000:
                /* Zero out our graphics array. */
                memset(chipper->graphics, 0, sizeof(chipper->graphics));
                chipper->PC += 2;
            break;

            case 0x000E:
                chipper->SP--;
                chipper->PC = chipper->stack[chipper->SP];
                chipper->PC += 2;
            break;
            }
        break;

        case 0x1000:
            chipper->PC = chipper->opcode & 0x0FFF;
        break;

        case 0x2000:
            chipper->stack[chipper->SP] = chipper->PC;
            chipper->SP++;
            chipper->PC = chipper->opcode & 0x0FFF;
        break;

        case 0x3000:
            if (chipper->V[(chipper->opcode & 0x0F00) >> 8] == (chipper->opcode & 0x00FF))
                chipper->PC += 4;
            else
                chipper->PC += 2;
        break;

        case 0x4000:
            if (chipper->V[(chipper->opcode & 0x0F00) >> 8] != (chipper->opcode & 0x00FF))
                chipper->PC += 4;
            else
                chipper->PC += 2;
        break;

        case 0x5000:
            if (chipper->V[(chipper->opcode & 0x0F00) >> 8] == chipper->V[(chipper->opcode & 0x00F0) >> 4])
                chipper->PC += 4;
            else
                chipper->PC += 2;
        break;

        case 0x6000:
            chipper->V[(chipper->opcode & 0x0F00) >> 8] = (chipper->opcode & 0x00FF);
            chipper->PC += 2;
        break;

        case 0x7000:
            chipper->V[(chipper->opcode & 0x0F00) >> 8] += (chipper->opcode & 0x00FF);
            chipper->PC += 2;
        break;

        case 0x8000:
            switch (chipper->opcode & 0x000F)
            {
            case 0x0000:
                chipper->V[(chipper->opcode & 0x0F00) >> 8] = chipper->V[(chipper->opcode & 0x00F0) >> 4];
                chipper->PC += 2;
            break;

            case 0x0001:
                chipper->V[(chipper->opcode & 0x0F00) >> 8] =
                    chipper->V[(chipper->opcode & 0x0F00) >> 8] | chipper->V[(chipper->opcode & 0x00F0) >> 4];
                chipper->PC += 2;
            break;

            case 0x0002:
                chipper->V[(chipper->opcode & 0x0F00) >> 8] =
                    chipper->V[(chipper->opcode & 0x0F00) >> 8] & chipper->V[(chipper->opcode & 0x00F0) >> 4];
                chipper->PC += 2;
            break;

            case 0x0003:
                chipper->V[(chipper->opcode & 0x0F00) >> 8] =
                    chipper->V[(chipper->opcode & 0x0F00) >> 8] ^ chipper->V[(chipper->opcode & 0x00F0) >> 4];
                chipper->PC += 2;
            break;

            case 0x0004:
                if (((int)chipper->V[(chipper->opcode & 0x0F00) >> 8] + (int)chipper->V[(chipper->opcode & 0x00F0) >> 4]) > 0xFF)
                    chipper->V[0xF] = 1;
                else
                    chipper->V[0xF] &= 0;
                
                chipper->V[(chipper->opcode & 0x0F00) >> 8] += chipper->V[(chipper->opcode & 0x00F0) >> 4];
                chipper->PC += 2;
            break;

            case 0x0005:
                if (((int)chipper->V[(chipper->opcode & 0x0F00) >> 8] - (int)chipper->V[(chipper->opcode & 0x00F0) >> 4]) >= 0)
                    chipper->V[0xF] = 1;
                else
                    chipper->V[0xF] &= 0;
                
                chipper->V[(chipper->opcode & 0x0F00) >> 8] -= chipper->V[(chipper->opcode & 0x00F0) >> 4];
                chipper->PC += 2;
            break;

            case 0x0006:
                chipper->V[0xF] = chipper->V[(chipper->opcode & 0x0F00) >> 8] & 0b00000001;
                chipper->V[(chipper->opcode & 0x0F00) >> 8] = chipper->V[(chipper->opcode & 0x00F0) >> 4] >> 1;
                chipper->PC += 2;
            break;

            case 0x0007:
                if (((int)chipper->V[(chipper->opcode & 0x0F00) >> 8] - (int)chipper->V[(chipper->opcode & 0x00F0) >> 4]) >= 0)
                    chipper->V[0xF] = 1;
                else
                    chipper->V[0xF] &= 0;
                
                chipper->V[(chipper->opcode & 0x0F00) >> 8] = chipper->V[(chipper->opcode & 0x0F00) >> 8] - chipper->V[(chipper->opcode & 0x00F0) >> 4];
                chipper->PC += 2;
            break;

            case 0x000E:
                chipper->V[0xF] = chipper->V[(chipper->opcode & 0x0F00) >> 8] >> 7;
                chipper->V[(chipper->opcode & 0x0F00) >> 8] = chipper->V[(chipper->opcode & 0x0F00) >> 8] << 1;
                chipper->PC += 2;
            break;

            default:
                printf("Wrong opcode: %04X\n", chipper->opcode);
            break;
            }
        break;

        case 0x9000:
            if (chipper->V[(chipper->opcode & 0x0F00) >> 8] != chipper->V[(chipper->opcode & 0x00F0) >> 4])
                chipper->PC += 4;
            else
                chipper->PC += 2;
        break;

        case 0xA000:
            chipper->I = chipper->opcode & 0x0FFF;
            chipper->PC += 2;
        break;

        case 0xB000:
            chipper->PC = (chipper->opcode & 0x0FFF) + chipper->V[0];
        break;

        case 0xC000:
            chipper->V[(chipper->opcode & 0x0F00) >> 8] = rand() & (chipper->opcode & 0x00FF);
            chipper->PC += 2;
        break;

        case 0xD000:
            /* Get our coordinates and height of the sprite from the opcode. */
            x = chipper->V[(chipper->opcode & 0x0F00) >> 8];
            y = chipper->V[(chipper->opcode & 0x00F0) >> 4];
            h = chipper->opcode & 0x000F;
            chipper->V[0xF] &= 0;

            /* For each line of the sprite, get the byte starting at memory location I. */
            for (int yline = 0; yline < h && (yline + y) < GRAPHICS_H; yline++)
            {
                sprite_row = chipper->memory[chipper->I + yline];
                /* Check each bit in the sprite line byte and draw it if it is 1. VF is set
                   if the sprite bit is 1 and there is already a pixel drawn. If not it XORs
                   the pixel onto the screen. */
                for (int xline = 0; xline < 8 && (xline + x) < GRAPHICS_W; xline++)
                    if (sprite_row & (0x80 >> xline))
                    {
                        if (chipper->graphics[x + xline + (y + yline) * GRAPHICS_W])
                            chipper->V[0xF] = 1;
                        chipper->graphics[x + xline + (y + yline) * GRAPHICS_W] ^= 0xFFFFFFFF;
                    }
            }
            chipper->PC += 2;
        break;

        case 0xE000:
            switch (chipper->opcode & 0x000F)
            {
            case 0x000E:
                keystate = SDL_GetKeyboardState(NULL);
                if (keystate[keymap[chipper->V[(chipper->opcode & 0x0F00) >> 8]]])
                    chipper->PC += 4;
                else
                    chipper->PC += 2;
            break;

            case 0x0001:
                keystate = SDL_GetKeyboardState(NULL);
                if (!keystate[keymap[chipper->V[(chipper->opcode & 0x0F00) >> 8]]])
                    chipper->PC += 4;
                else
                    chipper->PC += 2;
            break;

            default:
                printf("Wrong opcode: %04X", chipper->opcode);
            break;
            }
        break;

        case 0xF000:
            switch (chipper->opcode & 0x00FF)
            {
            case 0x0007:
                chipper->V[(chipper->opcode & 0x0F00) >> 8] = chipper->DT;
                chipper->PC += 2;
            break;

            case 0x000A:
                /* We simulate a halt / infinite loop by not progressing the
                   program counter until a key is pressed. This is so that it
                   keeps running this opcode until a key is pressed, which is
                   then stored in VX. */
                keystate = SDL_GetKeyboardState(NULL);
                for (int i = 0; i < 0x10; i++)
                    if (keystate[keymap[i]])
                    {
                        chipper->V[(chipper->opcode & 0x0F00) >> 8] = i;
                        chipper->PC += 2;
                    }
            break;

            case 0x0015:
                chipper->DT = chipper->V[(chipper->opcode & 0x0F00) >> 8];
                chipper->PC += 2;
            break;

            case 0x0018:
                chipper->ST = chipper->V[(chipper->opcode & 0x0F00) >> 8];
                chipper->PC += 2;
            break;

            case 0x001E:
                chipper->I += chipper->V[(chipper->opcode & 0x0F00) >> 8];
                chipper->PC += 2;
            break;

            case 0x0029:
                /* We multiply by 5 here since each font sprite is 5 bytes in length,
                   so to get to the right font sprite we need to take the number of
                   the sprite (1, 4, A, etc.) and multiply it by the font sprite width. */
                chipper->I = chipper->V[(chipper->opcode & 0x0F00) >> 8] * 5;
                chipper->PC += 2;
            break;

            case 0x0033:
                chipper->memory[chipper->I] = chipper->V[(chipper->opcode & 0x0F00) >> 8] / 100;
                chipper->memory[chipper->I + 1] = (chipper->V[(chipper->opcode & 0x0F00) >> 8] / 10) % 10;
                chipper->memory[chipper->I + 2] = chipper->V[(chipper->opcode & 0x0F00) >> 8] % 10;
                chipper->PC += 2;
            break;

            case 0x0055:
                for (int i = 0; i <= ((chipper->opcode & 0x0F00) >> 8); i++)
                    chipper->memory[chipper->I + i] = chipper->V[i];
                chipper->PC += 2;
            break;

            case 0x0065:
                for (int i = 0; i <= ((chipper->opcode & 0x0F00) >> 8); i++)
                    chipper->V[i] = chipper->memory[chipper->I + i];
                chipper->PC += 2;
            break;

            default:
                printf("Wrong opcode: %04X", chipper->opcode);
            break;
            }
        break;

        default:
            printf("ERROR UNKNOWN OPCODE\n");
        break;
        }
        ChipperTimers(chipper);
    }
}