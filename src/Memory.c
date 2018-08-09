#include <stdint.h>

/* Program internal memeory */
uint8_t  memory[4096];
uint16_t stack[16];     /* Used to store addresses for which the PC returns to after a subroutine */