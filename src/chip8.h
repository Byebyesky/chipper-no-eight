#ifndef CHIP8_H
#define CHIP8_H
//#define DEBUG

#include <cstdint>
#include <chrono>
#include "renderer.h"

static constexpr int MAXFILESIZE = 2048;
static constexpr int MINFILESIZE = 0;
static constexpr int ROMSTARTADDR = 0x200;
static constexpr int SCREENWIDTH = 64;
static constexpr int SCREENHEIGHT = 32;
static constexpr int SCREENSIZE = SCREENWIDTH*SCREENHEIGHT;

enum Keys   {
        KEY_0, KEY_1, KEY_2,
        KEY_3, KEY_4, KEY_5,
        KEY_6, KEY_7, KEY_8,
        KEY_9, KEY_A, KEY_B,
        KEY_C, KEY_D, KEY_E,
        KEY_F, KEY_NONE 
};

static constexpr uint8_t font[] = {
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

class Chip8 {
    private:

    int romSize;
    uint8_t memory[4096];
    uint16_t V[16], delayTimer, soundTimer, pc, sp, stack[24];
    uint32_t I;
    uint8_t frameBuffer[SCREENSIZE];
    uint32_t instructionCount;
    bool quit;
    bool resetFlag;
    SDL_Event event;
    Keys pressedKey = Keys::KEY_NONE;

    uint16_t fetchOpcode();
    void interpretOpcode(uint16_t opcode);
    void loadRom(char* fileName);
    void printMemHex(uint8_t *mem, std::size_t size);
    uint16_t getDelay();
    void reset();
    void initFont();
    uint8_t handleInput(bool &quit, bool &resetFlag);
    void decremtenTimers();
    void updateFramebuffer(uint8_t x, uint8_t y, uint8_t height, uint8_t *spritePtr);
    
    Renderer sdlRend;
    void initSDL();
    public:
    Chip8();
    void run(char *romName);

};

#endif