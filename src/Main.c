#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
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
    fseek(ROM, 0, SEEK_END);
    const unsigned int rom_size = ftell(ROM);
    fseek(ROM, 0, SEEK_SET);

    uint8_t *rom_buffer = malloc(sizeof(uint8_t) * rom_size);
    fread(rom_buffer, 1, rom_size, ROM);
    fclose(ROM);

    return 0;
}