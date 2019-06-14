#include "chip8.h"
#include <cstdio>
#include <fstream>
#include <stdlib.h>

using namespace std;

Chip8::Chip8() {
    reset();
}

void Chip8::initFont() {
    memcpy(memory, font, sizeof(font));
}

void Chip8::reset() {
    srand(time(NULL));
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));
    memset(V, 0, sizeof(V));
    memset(frameBuffer, 0, sizeof(frameBuffer));
    delayTimer = 0;
    soundTimer = 0;
    pc = ROMSTARTADDR;
    sp = 0;
    I = 0;
    instructionCount = 0;
    clk = chrono::steady_clock::now();
    initFont();
    sdlRend.clearDisplay();
    resetFlag = false;
    quit = false;

}

void Chip8::run(char *romName) {
    loadRom(romName);
    while(!quit) {
        interpretOpcode(fetchOpcode());
        std::chrono::steady_clock::time_point end = chrono::steady_clock::now();
        chrono::steady_clock::duration d = end - clk;
        if(d.count() >= speed) decremtentTimers();
        handleInput(quit, resetFlag);
        SDL_Delay(1);
        if(resetFlag) {reset(); loadRom(romName);SDL_Delay(200);}
        instructionCount++;
    }
    sdlRend.deInit();
}

//Fixing timers will fix speed inconsitencies
void Chip8::decremtentTimers() {
    if(delayTimer > 0) {
        delayTimer--;
    }
    if(soundTimer > 0) {
        soundTimer--;
        if(soundTimer == 0) printf("Beep!\n");
    }
    clk = chrono::steady_clock::now();
}

uint8_t Chip8::handleInput(bool &quit, bool &resetFlag) {
    SDL_PollEvent(&event);    
    pressedKey = Keys::KEY_NONE;
    const unsigned char* keystate = SDL_GetKeyboardState(NULL);

    if(keystate[SDL_SCANCODE_1])            pressedKey = Keys::KEY_1;
    else if(keystate[SDL_SCANCODE_2])       pressedKey = Keys::KEY_2;
    else if(keystate[SDL_SCANCODE_3])       pressedKey = Keys::KEY_3;
    else if(keystate[SDL_SCANCODE_4])       pressedKey = Keys::KEY_C;
    else if(keystate[SDL_SCANCODE_Q])       pressedKey = Keys::KEY_4;
    else if(keystate[SDL_SCANCODE_W])       pressedKey = Keys::KEY_5;
    else if(keystate[SDL_SCANCODE_E])       pressedKey = Keys::KEY_6;
    else if(keystate[SDL_SCANCODE_R])       pressedKey = Keys::KEY_D;
    else if(keystate[SDL_SCANCODE_A])       pressedKey = Keys::KEY_7;
    else if(keystate[SDL_SCANCODE_S])       pressedKey = Keys::KEY_8;
    else if(keystate[SDL_SCANCODE_D])       pressedKey = Keys::KEY_9;
    else if(keystate[SDL_SCANCODE_F])       pressedKey = Keys::KEY_E;
    else if(keystate[SDL_SCANCODE_X])       pressedKey = Keys::KEY_A;
    else if(keystate[SDL_SCANCODE_C])       pressedKey = Keys::KEY_0;
    else if(keystate[SDL_SCANCODE_V])       pressedKey = Keys::KEY_B;
    else if(keystate[SDL_SCANCODE_B])       pressedKey = Keys::KEY_F;

    
    switch (event.type)
    {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_TAB:
                    resetFlag = true;
                    break;
                case SDLK_KP_PLUS:
                    speed += 10000000;
                    printf("Speed is now: %ld\n", speed);
                    break;
                case SDLK_KP_MINUS:
                    speed -= 10000000;
                    printf("Speed is now: %ld\n", speed);
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    SDL_FlushEvent(SDL_KEYDOWN);
    return pressedKey;
}

void Chip8::loadRom(char *fileName) {
    ifstream rom;
    rom.open(fileName, ios::binary);
    if(rom.is_open()) {
        rom.seekg (0, ios::end);
        romSize = rom.tellg();
        if(romSize > MINFILESIZE && romSize <= MAXFILESIZE) {
            rom.seekg (0, ios::beg);
            rom.read((char*)memory+ROMSTARTADDR, romSize);
            fprintf(stderr, "Read file with %d bytes size\n", romSize);
        } else {
            fprintf(stderr, "File needs to be between %d to %d bytes\n", MINFILESIZE, MAXFILESIZE);
            quit = true;
        }
        rom.close();
    }
    else {
        fprintf(stderr, "Can't open file %s\n", fileName);
        quit = true;
    }
}

uint16_t Chip8::fetchOpcode() {
    uint16_t opcode = memory[pc+1] | memory[pc] << 8;
    pc += 2;
    return opcode;
}

void Chip8::interpretOpcode(uint16_t opcode) {
    uint8_t instruction = opcode >> 12 & 0xF;
    uint8_t x = opcode >> 8 & 0xF;
    uint8_t y = opcode >> 4 & 0xF;
    uint8_t n = opcode & 0xF;

    char debugString[50];
    sprintf(debugString, "0x%04x: ", pc-2);
    switch (instruction)
    {
        case 0x0:
            if(opcode == 0x00E0) {
                memset(frameBuffer, 0, sizeof(frameBuffer));
                sdlRend.clearDisplay();
                sprintf(debugString, "Clear()\n");
            } else if (opcode == 0x00EE) {
                sp--;
                if(sp >= 0) {
                    pc = stack[sp];
                } else {
                    fprintf(stderr, "Out of bounds stack read: %d!\n", sp);
                    exit(3);
                }
                sprintf(debugString, "RET to: %04x\n", pc);
            } else {
                fprintf(stderr, "Unknown instruction: 0x%04x at 0x%04x\n", opcode, pc);
            }
            break;
        case 0x1:
            pc = x << 8 | y << 4 | n;
            sprintf(debugString, "pc is now: %04x\n", pc);
            break;
        case 0x2:
            if(sp < 26) {
                stack[sp] = pc;
            } else {
                fprintf(stderr, "Out of bounds stack write: %d!\n", sp);
                exit(2);
            }
            sprintf(debugString, "Goto sub %03x | PC on stack: %04x\n", x << 8 | y << 4 | n, stack[sp]);
            sp++;
            pc = x << 8 | y << 4 | n;
            break;
        
        case 0x3:
            sprintf(debugString, "V%d (0x%0x) == 0x%0x\n", x, V[x], (y << 4 | n));
            if(V[x] == (y << 4 | n)) pc += 2;
            break;

        case 0x4:
            sprintf(debugString, "V%d (0x%0x) != 0x%0x\n", x, V[x], (y << 4 | n));
            if(V[x] != (y << 4 | n)) pc += 2;
            break;

        case 0x5:
            sprintf(debugString, "V%d (0x%0x) != V%d (0x%0x)\n", x, V[x], y, V[y]);
            if(V[x] == V[y]) pc += 2;
            break;

        case 0x6:
            V[x] = (y << 4 | n);
            V[x] &= 0xFF;
            sprintf(debugString, "V%d (0x%x) = 0x%x\n", x, V[x], (y << 4 | n));
            break;

        case 0x7:
            V[x] += (y << 4 | n);
            V[x] &= 0xFF;
            sprintf(debugString, "V%x (0x%x) + 0x%x = 0x%x\n", x, V[x], (y << 4 | n), V[x]);
            break;

        case 0x8:
            sprintf(debugString, "V%x (0x%x) | V%x (0x%x)", x, V[x], y, V[y]);
            switch (n) {
                case 0x0:
                    V[x] = V[y];
                    break;
                case 0x1:
                    V[x] |= V[y];
                    break;
                case 0x2:
                    V[x] &= V[y];
                    break;
                case 0x3:
                    V[x] ^= V[y];
                    break;
                case 0x4:
                    V[x] += V[y];
                    V[0xf] = (V[x] & (1 << 8)) >> 8;
                    V[x] &= 0xFF;
                    break;
                case 0x5:
                    V[x] -= V[y];
                    V[0xf] = !((V[x] & (1 << 8)) >> 8);
                    V[x] &= 0xFF;
                    break;
                case 0x6:
                    V[0xf] = V[x] & 1;
                    V[x] = V[x] >> 1;
                    break;
                case 0x7:
                    V[x] = V[y]- V[x];
                    V[0xf] = !((V[x] & (1 << 8)) >> 8);
                    V[x] &= 0xFF;
                    break;
                case 0xE:
                    V[0xf] = (V[x] & (1 << 7)) >> 7;
                    V[x] = (V[x] << 1) & 0xFF;
                    break;

                default:
                    break;
            }
            sprintf(debugString, " VF: %d V%x == (%x)\n", V[0xF], x, V[x]);
            break;

        case 0x9:
            sprintf(debugString, "V%x (0x%0x) != V%x (0x%0x)\n", x, V[x], y, V[y]);
            if(V[x] != V[y]) pc += 2;
            break;

        case 0xa:
            I = x << 8 | y << 4 | n;
            sprintf(debugString, "I is now %04x\n", I);
            break;

        case 0xb:
            pc = (x << 8 | y << 4 | n) + V[0];
            sprintf(debugString, "pc is now: %04x\n", pc);
            break;

        case 0xc:
            V[x] = rand() & (y << 4 | n);
            V[x] &= 0xFF;
            sprintf(debugString, "V%1x is now: %d\n", x, V[x]);
            break;


        case 0xd:
            sprintf(debugString, "draw(%d, %d, %d) from 0x%04x (0x%04x)\n", V[x], V[y], n, I, I > 0x200 ? I-ROMSTARTADDR : 0 );
            if (I + n < MEMORYSIZE) {
                updateFramebuffer(V[x], V[y], n, &memory[I]);
            } else {
                fprintf(stderr, "Out of bounds memory write: %d!", I);
                exit(4);
            }
            
            sdlRend.drawPixel(V[x], V[y], n, frameBuffer);
            break;

        case 0xe:
            if(((y << 4) | n) == 0x9E ) {
                sprintf(debugString, "pressedKey (%x) == V%1x (%x)\n", pressedKey, x, V[x]);
                if(V[x] == pressedKey) pc += 2;
            } else if(((y << 4) | n) == 0xA1 ) {
                sprintf(debugString, "pressedKey (%x) != V%1x (%x)\n", pressedKey, x, V[x]);
                if(V[x] != pressedKey) pc += 2;
            } else {
                fprintf(stderr, "Unknown instruction: %04x at %04x\n", opcode, pc);
            }
            break;

        case 0xf:
            switch((y << 4) | n) {
                case 0x7:
                    V[x] = delayTimer;
                    break;
                case 0xa:
                    while(pressedKey == Keys::KEY_NONE) {
                        handleInput(quit, resetFlag);
                        if(pressedKey != Keys::KEY_NONE) V[x] = pressedKey;
                        if(quit || resetFlag) break;
                        SDL_Delay(5);
                    }
                    break;
                case 0x15:
                    delayTimer = V[x];
                    break;
                case 0x18:
                    soundTimer = V[x];
                    break;
                case 0x1e:
                    I += V[x];
                    V[0xf] = (I & (1 << 16)) ? 1 : 0;
                    I &= 0xFFFF;
                    break;
                case 0x29:
                    I = V[x]*5;
                    break;
                case 0x33:
                    if(I+2 < MEMORYSIZE) {
                        memory[I] = V[x] / 100;
                        memory[I+1] = (V[x] % 100) / 10;
                        memory[I+2] = V[x] % 10;
                    } else {
                        fprintf(stderr, "Out of bounds memory write: %d\n", I);
                        exit(4);
                    }
                    break;
                case 0x55:
                    for(int i = 0; i <= x; i++) {
                        if(I+i < MEMORYSIZE) {
                            memory[I + i] = V[i];
                        } else {
                            fprintf(stderr, "Out of bounds memory write: %d\n", I);
                            exit(4);
                        }
                    }
                    break;
                case 0x65:
                    for(int i = 0; i <= x; i++) {
                        if(I+i < MEMORYSIZE) {
                            V[i] = memory[I + i];
                        } else {
                            fprintf(stderr, "Out of bounds memory read: %d\n", I);
                            exit(5);
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            fprintf(stderr, "Unimplemented opcode 0x%04x at 0x%04x\n", opcode, pc);
            break;
    }

    #ifdef DEBUG
    printf("%s", debugString);
    #endif
}

void Chip8::updateFramebuffer(uint8_t x, uint8_t y, uint8_t n, uint8_t *spritePtr) {
    int offset = x+(y*SCREENWIDTH);
    if(offset > SCREENSIZE) {
        fprintf(stderr, "Out of bounds frame write: %d\n", offset);
        exit(6);
    }
    constexpr int width = 8;
    V[0xf] = 0;

    for(int i = 0; i < n; i++) {
        uint8_t pixels = *spritePtr; //Get 8 pixels
        for(int j =0; j < width; j++) {
            if(pixels & (1 << 7)) frameBuffer[offset+j+i*SCREENWIDTH] ^= 1;
            if(!frameBuffer[offset+j+i*SCREENWIDTH]) V[0xf] = 1;
            pixels = pixels << 1;
        }
        spritePtr = spritePtr+1;
    }
}

void Chip8::printMemHex(uint8_t *mem, size_t size){
    for(int i = 0; i < size; i++) {
        if(i % 16 == 0) printf("\n0x%04x: ", i);
        printf("%02x ", mem[i]);
    }
    printf("\n\n");
}