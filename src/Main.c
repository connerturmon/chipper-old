#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    /* Just testing bitwork */
    uint16_t num = 0xFA32;
    uint8_t  val = (num & 0x0F00) >> 8;

    printf("%u", val);

    return 0;
}