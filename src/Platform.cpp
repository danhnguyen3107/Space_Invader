#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "include/Platform.h"
#include <bitset>

Platform::Platform(){
    // Initialize SDL (video and audio).
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << "\n";
        return;
    }
    
    // Create a dummy window (required by some SDL functionality)
    SDL_CreateWindowAndRenderer(224, 256, 0, &window, &renderer);

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return;
    }

}


Platform::~Platform(){
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Initialize(I8080* cpu, IO_device* io) {
    this->cpu = cpu;
    this->io = io;
}

void  Platform::process(I8080* cpu, IO_device* io, uint8_t* checkSound) {
    if ((cpu->InPort[1] & (1 << 0)) != 0) {
        cpu->OutPort[3] = 0x01;
        std::cout << "credit" << std::endl;
    }

    if ((cpu->InPort[1] & (1 << 1)) != 0) {
        cpu->OutPort[3] = 0x08;
        std::cout << "2P start " << std::endl;
    }

    if ((cpu->InPort[1] & (1 << 2)) != 0) {
        cpu->OutPort[3] = 0x04;
        std::cout << "1P start " << std::endl;
    }

    if ((cpu->InPort[1] & (1 << 4)) != 0) {
        cpu->OutPort[3] = 0x02;
        std::cout << "1P shot " << std::endl;
    }

    if ((cpu->InPort[1] & (1 << 5)) != 0) {
        cpu->OutPort[5] = 0x01;
        std::cout << "1P left " << std::endl;
    }

    if ((cpu->InPort[1] & (1 << 6)) != 0) {
        cpu->OutPort[5] = 0x02;
        std::cout << "1P right " << std::endl;
    }

    // cpu->OutPort[3] = 0x00;

    io->output(cpu);

}




SDL_Color Platform::calculateOverlay(uint8_t hor, uint8_t ver){
#define WHITE {255,255,255}
#define RED   {255,  0,  0}
#define GREEN {  0,255,  0}
    // Default Space invaders game overlay
    if (ver >= 256 - 32) { return WHITE; }
    if (ver >= (256 - 32 - 32)) { return RED; }
    if (ver >= (256 - 32 - 32 - 120)) { return WHITE; }
    if (ver >= (256 - 32 - 32 - 120 - 56)) { return GREEN; }
    // Last horizontal region divided in 3 parts
    if (hor <= 16) { return WHITE; }
    if (hor <= (16 + 118)) { return GREEN; }
    return WHITE;
}

void Platform::DrawSprite() {
    // Draw the sprite based on the bit mask.
    SDL_Color pixelColor;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    // Draw the pixels from the memory locations 0x2400 - 0x3fff
    // into the window screen
    for(uint16_t v=0; v<224; v++){
        for(int16_t h=0; h<256; h++){
            uint16_t base_offset = 0x2400;
            uint16_t vertical_offset = 0x20 * v;
            uint16_t horizontal_offset = (h >> 3);
            uint16_t current_byte = base_offset
                                + vertical_offset
                                + horizontal_offset;
            uint8_t current_bit = (h % 8);

            bool thisPixel = (cpu->memory[current_byte] & (1 << current_bit)) != 0;

            // retrieve the current pixel color
            if(thisPixel){
                pixelColor = calculateOverlay(v, h);
                SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, 255);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            }

            // Rotate coordinates counter clockwise
            SDL_RenderDrawPoint(renderer, v, 256-h-1);
        }
    }
    SDL_RenderPresent(renderer);

}



void Platform::EmulatorCycle() {

    // Main loop.
    bool running = true;

    uint8_t checkSound = 0x00;

    uint32_t lastFrameTime = SDL_GetTicks();
    uint32_t currentTime = SDL_GetTicks();

    std::cout << "Starting emulation..." << std::endl;
    while (running && !cpu->halt) {
        currentTime = SDL_GetTicks();
        if (currentTime - lastFrameTime >= 16) {
            lastFrameTime = currentTime;
            cpu->Cycle(16666); // 60Hz

            cpu->Interrupts(0xcf); // RST 1
            cpu->Cycle(16666);

            cpu->Interrupts(0xd7); // RST 2
            DrawSprite();
        } 

        // Handle input from the keyboard.
        io->handle_input(cpu);
        io->output(cpu);
    

    }
}

