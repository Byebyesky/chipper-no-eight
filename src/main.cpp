#include <iostream>
#include "chip8.h"

using namespace std;

int main(int argc, char** argv) {
    
    if(argc < 2) {
        printf("Usage: %s romfile\n", argv[0]);
        return 1;
    }

    printf("Loading rom: %s\n", argv[1]);

    Chip8 c;

    c.run(argv[1]);
    return 0;
}