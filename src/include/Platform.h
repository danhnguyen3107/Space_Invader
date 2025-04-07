
// #ifndef PLATFORM_H
// #define PLATFORM_H

#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "I8080.h"
#include "IO_device.h"

class Platform {
public:

    SDL_Window* window = nullptr;


    I8080* cpu = nullptr;
    IO_device* io = nullptr;

    SDL_Renderer* renderer;

    Platform();
    ~Platform();

    void Initialize( I8080* cpu, IO_device* io );
    void EmulatorCycle();
    

    void process(I8080* cpu, IO_device* io, uint8_t* checkSound);
    SDL_Color calculateOverlay(uint8_t hor, uint8_t ver);
    void DrawSprite();
    
    
};


// #endif