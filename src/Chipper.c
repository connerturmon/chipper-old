#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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

static int keymap[0x10] = {
    SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,
    SDLK_a,
    SDLK_b,
    SDLK_c,
    SDLK_d,
    SDLK_e,
    SDLK_f
};

// This is the main the Chipper function that initializes a bunch of the
// required subsystems like SDL and the Interpreter.
void ChipperStart(const char *rom)
{
    // Start by initializing our interpreter for use.
    Chip8 chipper;
    ChipperInitialize(rom, &chipper);

    // Initialize SDL for graphics and user input.
    uint8_t keys;
    SDL_Event event;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetVideoMode(
        DISPLAY_W, DISPLAY_H, 
        DISPLAY_BPP,
        SDL_HWSURFACE | SDL_DOUBLEBUF
    );

    // Enter main loop of the program.
    for (;;)
    {
        if (SDL_PollEvent(&event))
            if (event.key.state == SDL_PRESSED
                && event.key.keysym.sym == SDLK_ESCAPE)
                exit(1);
        
        ChipperExecute(&chipper);
        ChipperDraw(&chipper);
    }
}

//------------------ SYSTEM FUNCTIONS ------------------//

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

void ChipperExecute(Chip8 *chipper)
{
    // Used for testing for overflows
    uint16_t result;

    uint8_t  *keys;
    int      y, x, vx, vy, times, i;
    unsigned height, pixel;

    // Begin main execution loop
    for (int cycles = 0; cycles <= 10; cycles++)
    {
        chipper->opcode = chipper->memory[chipper->registers.PC] << 8 |
            chipper->memory[chipper->registers.PC + 1];
        printf("OPCODE: %04X | PC: %02X | I: %04X | SP: %04X\n",
            chipper->opcode, chipper->registers.PC, chipper->registers.I, chipper->registers.SP);
        switch ((chipper->opcode & 0xF000) >> 12)
        {
        // Because there are mutiple operations with the '0x0' prefix, there
        // has to be another switch case to handle the sub-operations.
        case 0x0:
            switch (chipper->opcode & 0x00FF)
            {
            case 0xE0:
                // 0x00E0 : CLS
                break;
            case 0xEE:
                // 0x00EE : RET
                chipper->registers.PC = chipper->stack[chipper->registers.SP];
                chipper->registers.SP--;
                break;
            default:
                printf("UNEXPECTED OP\n");
                break;
            }
        break;
        
        case 0x1:
            // 1nnn : JMP addr
            chipper->registers.PC = (chipper->opcode & 0x0FFF);
            break;
        case 0x2:
            // 2nnn : CALL addr
            chipper->registers.SP++;
            chipper->stack[chipper->registers.SP] = chipper->registers.PC;
            chipper->registers.PC = (chipper->opcode & 0x0FFF);
            break;
        case 0x3:
            // 0x3xkk : SE Vx, byte
            if (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8 ] == (chipper->opcode & 0x00FF))
                chipper->registers.PC += 2;
            chipper->registers.PC += 2;
            break;
        case 0x4:
            // 0x4xkk : SNE Vx, byte
            if (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8 ] != (chipper->opcode & 0x00FF))
                chipper->registers.PC += 2;
            chipper->registers.PC += 2;
            break;
        case 0x5:
            // 0x5xy0 : SE Vx, Vy
            if (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8 ] == chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4 ])
                chipper->registers.PC += 2;
            chipper->registers.PC += 2;
            break;
        case 0x6:
            // 0x6xkk : LD Vx, byte
            chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] = chipper->opcode & 0x00FF;
            chipper->registers.PC += 2;
            break;
        case 0x7:
            // ADD Vx, byte
            chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] += chipper->opcode & 0x00FF;
            chipper->registers.PC += 2;
            break;
        
        case 0x8:
            switch (chipper->opcode & 0x000F)
            {
            case 0x0:
                // LD Vx, Vy
                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] =
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4];
                chipper->registers.PC += 2;
                break;
            case 0x1:
                // OR Vx, Vy
                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] |=
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4];
                chipper->registers.PC += 2;
                break;
            case 0x2:
                // AND Vx, Vy
                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] &=
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4];
                chipper->registers.PC += 2;
                break;
            case 0x3:
                // XOR Vx, Vy
                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] ^=
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4];
                chipper->registers.PC += 2;
                break;
            case 0x4:
                // ADD Vx, Vy
                result = (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8]) +
                    (chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4]);
                if (result > 0xFF)
                    chipper->registers.V[0xF] = 1;

                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] +=
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4];
                chipper->registers.PC += 2;
                break;
            case 0x5:
                // SUB Vx, Vy
                if (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] >
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4])
                    chipper->registers.V[0xF] = 1;
                else
                    chipper->registers.V[0xF] = 0;

                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8 ] -=
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4];
                chipper->registers.PC += 2;
                break;
            case 0x6:
                // SHR Vx
                if ( (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] & 0x1) == 1)
                    chipper->registers.V[0xF] = 1;
                else
                    chipper->registers.V[0xF] = 0;
                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] =
                    chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] >> 1;
                chipper->registers.PC += 2;
                break;
            case 0x7:
                // SUBN Vx, Vy
                if (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] <
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4])
                    chipper->registers.V[0xF] = 1;
                else
                    chipper->registers.V[0xF] = 0;

                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8 ] =
                    chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4] - chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8];
                chipper->registers.PC += 2;
                break;
            case 0xE:
                if ( (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] & 0x80) == 1)
                    chipper->registers.V[0xF] = 1;
                else
                    chipper->registers.V[0xF] = 0;
                chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] =
                    chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] << 1;
                chipper->registers.PC += 2;
                break;
            default:
                printf("UNEXPECTED OP\n");
                break;
            }
        break;

        case 0x9:
            // SNE Vx, Vy
            if (chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] !=
                chipper->registers.V[ (chipper->opcode & 0x00F0) >> 4])
                chipper->registers.PC += 2;
            chipper->registers.PC += 2;
            break;
        case 0xA:
            // Set I = nnn
            chipper->registers.I = (chipper->opcode & 0x0FFF);
            chipper->registers.PC += 2;
            break;
        case 0xB:
            // JP V0, addr
            chipper->registers.PC = (chipper->opcode & 0x0FFF) + chipper->registers.V[0];
            break;
        case 0xC:
            // RND Vx, byte
            chipper->registers.V[ (chipper->opcode & 0x0F00) >> 8] = rand() & (chipper->opcode & 0x00FF);
            chipper->registers.PC += 2;
            break;
        case 0xD:
            // DRW Vx, Vy, nibble
            // Get x and y coordinates to draw from Vx and Vy
            vx = chipper->registers.V[(chipper->opcode & 0x0F00) >> 8];
            vy = chipper->registers.V[(chipper->opcode & 0x00F0) >> 4];
            // The number of bytes to read in from memory starting at location I
            height = chipper->opcode & 0x000F;
            chipper->registers.V[0xF] &= 0;
            
            for(y = 0; y < height; y++){
                pixel = chipper->memory[chipper->registers.I + y];
                for(x = 0; x < 8; x++){
                    if(pixel & (0x80 >> x)){
                        if(chipper->graphics[x + vx + (y + vy) * 64])
                            chipper->registers.V[0xF] = 1;
                        chipper->graphics[ x + vx + (y + vy) * 64] ^= 1;
                    }
                }
            }
            chipper->registers.PC += 2;
            break;
        
        case 0xE:
            switch(chipper->opcode & 0x000F)
            {
                case 0x000E: // EX9E: Skips the next instruction if the key stored in VX is pressed
                    keys = SDL_GetKeyState(NULL);
                    if(keys[keymap[chipper->registers.V[(chipper->opcode & 0x0F00) >> 8]]])
                        chipper->registers.PC += 4;
                    else
                        chipper->registers.PC += 2;
                break;
                             
                case 0x0001: // EXA1: Skips the next instruction if the key stored in VX isn't pressed
                    keys = SDL_GetKeyState(NULL);
                    if(!keys[keymap[chipper->registers.V[(chipper->opcode & 0x0F00) >> 8]]])
                        chipper->registers.PC += 4;
                    else
                        chipper->registers.PC += 2;
                break;
                
                default: printf("Wrong opcode: %X\n", chipper->opcode); getchar(); break;     
            }
        break;
       
        case 0xF:
            switch(chipper->opcode & 0x00FF)
                {
                    case 0x0007: // FX07: Sets VX to the value of the delay timer
                        chipper->registers.V[(chipper->opcode & 0x0F00) >> 8] = chipper->registers.DT;
                        chipper->registers.PC += 2;
                    break;
       
                    case 0x000A: // FX0A: A key press is awaited, and then stored in VX
                        keys = SDL_GetKeyState(NULL);
                        for(i = 0; i < 0x10; i++)
                            if(keys[keymap[i]]){
                                chipper->registers.V[(chipper->opcode & 0x0F00) >> 8] = i;
                                chipper->registers.PC += 2;
                            }
                    break;
       
                    case 0x0015: // FX15: Sets the delay timer to VX
                        chipper->registers.DT = chipper->registers.V[(chipper->opcode & 0x0F00) >> 8];
                        chipper->registers.PC += 2;
                    break;
       
                    case 0x0018: // FX18: Sets the sound timer to VX
                        chipper->registers.ST = chipper->registers.V[(chipper->opcode & 0x0F00) >> 8];
                        chipper->registers.PC += 2;
                    break;
       
                    case 0x001E: // FX1E: Adds VX to I
                        chipper->registers.I += chipper->registers.V[(chipper->opcode & 0x0F00) >> 8];
                        chipper->registers.PC += 2;
                    break;

                    case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
                        chipper->registers.I = chipper->registers.V[(chipper->opcode & 0x0F00) >> 8] * 5;
                        chipper->registers.PC += 2;
                    break;
       
                    case 0x0033: // FX33: Stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2
                        chipper->memory[chipper->registers.I] = chipper->registers.V[(chipper->opcode & 0x0F00) >> 8] / 100;
                        chipper->memory[chipper->registers.I+1] = (chipper->registers.V[(chipper->opcode & 0x0F00) >> 8] / 10) % 10;
                        chipper->memory[chipper->registers.I+2] = chipper->registers.V[(chipper->opcode & 0x0F00) >> 8] % 10;
                        chipper->registers.PC += 2;
                    break;
       
                    case 0x0055: // FX55: Stores V0 to VX in memory starting at address I
                        for(i = 0; i <= ((chipper->opcode & 0x0F00) >> 8); i++)
                            chipper->memory[chipper->registers.I+i] = chipper->registers.V[i];
                        chipper->registers.PC += 2;
                    break;
       
                    case 0x0065: //FX65: Fills V0 to VX with values from memory starting at address I
                        for(i = 0; i <= ((chipper->opcode & 0x0F00) >> 8); i++)
                            chipper->registers.V[i] = chipper->memory[chipper->registers.I + i];
                        chipper->registers.PC += 2;
                    break;
                         
                    default: printf("Wrong opcode: %X\n", chipper->opcode); getchar(); break;
                }
        break;

        default: break;
        }
    }
}