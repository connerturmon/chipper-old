#include <stdio.h>
#include "Chipper.h"

int main(int argc, char* argv[])
{
    if (argc > 1)
        ChipperStart(argv[1]);
    else
        printf("ERROR Initializing: No ROM provided as argument...\n");

    return 0;
}