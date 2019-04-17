#ifndef RENDERER_H
#define RENDERER_H
#include <SDL2/SDL.h>

class Renderer {
    private:
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    uint8_t SCALE = 10;

    public:
    Renderer();
    Renderer(int scale);
    void drawPixel(uint8_t x, uint8_t y, uint8_t height, uint8_t *sprite);
    void deInit();
    void wait();
    void clearDisplay();
};

#endif