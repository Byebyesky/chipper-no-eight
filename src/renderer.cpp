#include "renderer.h"


Renderer::Renderer() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(
                                "Chipper No.8",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                64*SCALE,
                                32*SCALE,
                                SDL_WINDOW_OPENGL                            
                                );
    if(window == NULL) {
        printf("Couldn't create window %s\n", SDL_GetError());
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, 0);

    clearDisplay();

}

Renderer::Renderer(int scale) {
    SCALE = scale;
    Renderer();
}

void Renderer::clearDisplay() {
    SDL_SetRenderDrawColor(renderer, 0,0,0,255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

void Renderer::drawPixel(uint8_t x, uint8_t y, uint8_t height, uint8_t *spritePtr) {
    SDL_Rect r;
    r.x = x * SCALE;
    r.y = y * SCALE;
    r.w = 1 * SCALE;
    r.h = 1 * SCALE;

    for(int i = 0; i < 32; i++) {
        for(int j = 0; j < 64; j++) {
            r.x = j*SCALE;
            r.y = i*SCALE;
            if(*spritePtr) {
                SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
                SDL_RenderFillRect(renderer, &r);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0,0,0,255);
                SDL_RenderFillRect(renderer, &r);
            }
            spritePtr = spritePtr+1;
        }
    }

    SDL_RenderPresent(renderer);
}


void Renderer::deInit() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::wait() {
    SDL_Delay(1000/500);
}