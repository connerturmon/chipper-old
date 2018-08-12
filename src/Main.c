#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include "Interpreter.h"
#include "Instructions.h"
#include "Memory.h"

int main(int argc, char* argv[])
{
    /* Make sure that a ROM was passed to the program */
    if (argc < 2)
    {
        printf("Please pass a ROM file as CLI argument...\n");
        exit(900);
    }

    FILE *ROM = fopen(argv[1], "r");
    if (!ROM)
    {
        printf("Unable to open file...\n");
        exit(901);
    }
    /* Get size of the ROM to allocate space for buffer */
    fseek(ROM, 0, SEEK_END);
    const unsigned int rom_size = ftell(ROM);
    fseek(ROM, 0, SEEK_SET);

    /* Create the buffer and read in so that we can begin emulation */
    uint8_t *rom_buffer = malloc(sizeof(uint8_t) * rom_size);
    fread(rom_buffer, 1, rom_size, ROM);
    fclose(ROM);

    /* Initialize emulator for use */
    Registers cpu_registers;
    InitializeRegisters(&cpu_registers);
    uint8_t memory[4096];
    uint16_t stack[16];
    InitializeMemory(memory, stack);

    /* Copy memory from file buffer into main emulator memory */
    for (int i = 0; i < rom_size; i++)
        memory[0x200 + i] = rom_buffer[i];
    free(rom_buffer);

    unsigned char running = 1;
    uint16_t opcode;
    while (cpu_registers.PC < rom_size)
    {
        /* Fetch 2 Byte opcodes */
        opcode = memory[0x200 + cpu_registers.PC] << 8;
        opcode += memory[0x200 + cpu_registers.PC + 1];

        switch ((opcode & 0xF000) >> 12)
        {
        case 0x0:
            switch (opcode & 0x00FF)
            {
            case 0xE0:
                _CLS();
                printf("CLS\n");
                cpu_registers.PC += 2;
                break;
            case 0xEE:
                _RET(&cpu_registers, stack);
                printf("RET\n");
                break;
            default:
                printf("UNEXPECTED OPCODE IN 0x0yyy SET: %04X\n", opcode);
                cpu_registers.PC += 2;
                break;
            }

        case 0x1:
            _JP_addr(opcode, &cpu_registers);
            printf("JP ADDR\n");
            break;
        case 0x2:
            _CALL_addr(opcode, &cpu_registers, stack);
            printf("CALL ADDR\n");
            cpu_registers.PC += 2;
            break;
        case 0x3:
            _SE_Vx_B(opcode, &cpu_registers);
            printf("SE Vx B\n");
            cpu_registers.PC += 2;
            break;
        case 0x4:
            _SNE_Vx_B(opcode, &cpu_registers);
            printf("SNE Vx B\n");
            cpu_registers.PC += 2;
            break;
        case 0x5:
            _SE_Vx_Vy(opcode, &cpu_registers);
            printf("SE Vx Vy\n");
            cpu_registers.PC += 2;
            break;
        case 0x6:
            _LD_Vx_B(opcode, &cpu_registers);
            printf("LD Vx B\n");
            cpu_registers.PC += 2;
            break;
        case 0x7:
            _ADD_Vx_B(opcode, &cpu_registers);
            printf("ADD Vx B\n");
            cpu_registers.PC += 2;
            break;

        case 0x8:
            switch (opcode & 0x000F)
            {
            case 0x0:
                _LD_Vx_Vy(opcode, &cpu_registers);
                printf("LD Vx Vy\n");
                cpu_registers.PC += 2;
                break;
            case 0x1:
                _OR_Vx_Vy(opcode, &cpu_registers);
                printf("OR Vx Vy\n");
                cpu_registers.PC += 2;
                break;
            case 0x2:
                _AND_Vx_Vy(opcode, &cpu_registers);
                printf("AND Vx Vy\n");
                cpu_registers.PC += 2;
                break;
            case 0x3:
                _XOR_Vx_Vy(opcode, &cpu_registers);
                printf("XOR Vx Vy\n");
                cpu_registers.PC += 2;
                break;
            case 0x4:
                _ADD_Vx_Vy(opcode, &cpu_registers);
                printf("ADD Vx Vy\n");
                cpu_registers.PC += 2;
                break;
            case 0x5:
                _SUB_Vx_Vy(opcode, &cpu_registers);
                printf("SUB Vx Vy\n");
                cpu_registers.PC += 2;
                break;
            case 0x6:
                _SHR_Vx(opcode, &cpu_registers);
                printf("SHR Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0x7:
                _SUBN_Vx_Vy(opcode, &cpu_registers);
                printf("SUBN Vx Vy\n");
                cpu_registers.PC += 2;
                break;
            case 0xE:
                _SHL_Vx(opcode, &cpu_registers);
                printf("SHL Vx\n");
                cpu_registers.PC += 2;
                break;
            default:
                printf("UNEXPECTED OPCODE IN 0x8yyy SECTION: %04X\n", opcode);
                cpu_registers.PC += 2;
                break;
            }

        case 0x9:
            _SNE_Vx_Vy(opcode, &cpu_registers);
            printf("SNE Vx Vy\n");
            cpu_registers.PC += 2;
            break;
        case 0xA:
            _LD_I_addr(opcode, &cpu_registers);
            printf("LD I ADDR\n");
            cpu_registers.PC += 2;
            break;
        case 0xB:
            _JP_V0_addr(opcode, &cpu_registers);
            printf("JP V0 ADDR\n");
            break;
        case 0xC:
            _RND_Vx_B();
            printf("UNIMPLEMENTED: RND Vx B\n");
            cpu_registers.PC += 2;
            break;
        case 0xD:
            _DRW_Vx_Vy_nibble();
            printf("UNIMPLEMENTED: DRW Vx Vy nibble\n");
            cpu_registers.PC += 2;
            break;

        case 0xE:
            switch (opcode & 0x00FF)
            {
            case 0x9E:
                _SKP_Vx();
                printf("UNIMPLEMENTED: SKP Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0xA1:
                _SKNP_Vx();
                printf("UNIMPLEMENTED: SKNP Vx\n");
                cpu_registers.PC += 2;
                break;
            default:
                printf("UNEXPECTED OPCODE IN 0xEyyy SECTION: %04X\n", opcode);
                cpu_registers.PC += 2;
                break;
            }

        case 0xF:
            switch (opcode & 0x00FF)
            {
            case 0x07:
                _LD_Vx_DT(opcode, &cpu_registers);
                printf("LD Vx DT\n");
                cpu_registers.PC += 2;
                break;
            case 0x0A:
                _LD_Vx_K();
                printf("UNIMPLEMENTED: LD Vx K\n");
                cpu_registers.PC += 2;
                break;
            case 0x15:
                _LD_DT_Vx(opcode, &cpu_registers);
                printf("LD DT Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0x18:
                _LD_ST_Vx(opcode, &cpu_registers);
                printf("LD ST Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0x1E:
                _ADD_I_Vx(opcode, &cpu_registers);
                printf("ADD I Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0x29:
                _LD_F_Vx();
                printf("UNIMPLEMENTED: LD F Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0x33:
                _LD_B_Vx();
                printf("UNIMPLEMENTED: LD B Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0x55:
                _LD_addrI_Vx(opcode, &cpu_registers, memory);
                printf("LD ADDR_I Vx\n");
                cpu_registers.PC += 2;
                break;
            case 0x65:
                _LD_Vx_addrI(opcode, &cpu_registers, memory);
                printf("LD Vx ADDR_I\n");
                cpu_registers.PC += 2;
                break;
            default:
                printf("UNEXPECTED OPCODE IN 0xFyyy SECTION: %04X\n", opcode);
                cpu_registers.PC += 2;
                break;
            }

        default:
            printf("UNEXPECTED OPERATION CODE: %04X", opcode);
            cpu_registers.PC += 2;
            break;
        }

    }

    return 0;
}