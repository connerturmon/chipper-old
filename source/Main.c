#include <stdio.h>
#include "Chipper.h"

int main(int argc, char *argv[])
{
    /* Check to see if a ROM is passed as a paremeter to the program,
       and then start Chipper. */
    if (argc < 2)
        printf("Error: No ROM Passed as a parameter.\n");
    else
        ChipperStart(argv[1]);

    return 0;
}