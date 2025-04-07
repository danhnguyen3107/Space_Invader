#ifndef IO_DEVICE_H
#define IO_DEVICE_H

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "I8080.h"

class IO_device {
public:
    IO_device();
    // ~IO_device();
    

    Mix_Chunk *UFOSound = nullptr;
    Mix_Chunk *Shoot = nullptr;
    Mix_Chunk *PlayerDeath = nullptr;
    Mix_Chunk *InvaderDeath = nullptr;

    Mix_Chunk *Fleet1 = nullptr;
    Mix_Chunk *Fleet2 = nullptr;
    Mix_Chunk *Fleet3 = nullptr;
    Mix_Chunk *Fleet4 = nullptr;

    uint8_t ch_Sound;

    // Function to handle key events and change the color of the sprite based on the key pressed
    void handle_input(I8080 *cpu);
    void output(I8080 *cpu);
    void playSound(uint8_t id);
};

#endif // IO_DEVICE_H