
#include <iostream>
#include <SDL2/SDL.h>
#include "include/IO_device.h"



IO_device::IO_device() {

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
        SDL_Quit();
        return;
    }


	UFOSound = Mix_LoadWAV("Sound/Sounds_ufo_highpitch.wav");
	Shoot = Mix_LoadWAV("Sound/Sounds_shoot.wav");
	PlayerDeath = Mix_LoadWAV("Sound/Sounds_explosion.wav");
	InvaderDeath = Mix_LoadWAV("Sound/Sounds_invaderkilled.wav");
	Fleet1 = Mix_LoadWAV("Sound/Sounds_fastinvader1.wav");
	Fleet2 = Mix_LoadWAV("Sound/Sounds_fastinvader2.wav");
	Fleet3 = Mix_LoadWAV("Sound/Sounds_fastinvader3.wav");
	Fleet4 = Mix_LoadWAV("Sound/Sounds_fastinvader4.wav");

    if (!UFOSound ) {
        std::cerr << "Failed to load UFOSound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }

    if (!Shoot ) {
        std::cerr << "Failed to load Shoot sound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
    if (!PlayerDeath ) {
        std::cerr << "Failed to load PlayerDeath sound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
    if (!InvaderDeath ) {
        std::cerr << "Failed to load InvaderDeath sound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
    if (!Fleet1 ) {
        std::cerr << "Failed to load Fleet1 sound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
    if (!Fleet2 ) {
        std::cerr << "Failed to load Fleet2 sound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
    if (!Fleet3 ) {
        std::cerr << "Failed to load Fleet3 sound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }
    if (!Fleet4 ) {
        std::cerr << "Failed to load Fleet4 sound! SDL_mixer Error: " << Mix_GetError() << "\n";
    }


}


void IO_device::handle_input(I8080 *cpu) {
    // Poll for events.
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            cpu->halt = 1;
        }
        // Process only keydown events.
        else if (event.type == SDL_KEYDOWN) {
            // Cleanup before processing new key input.
            // cpu->InPort[0] &= 0b10001111;
            // cpu->InPort[1] &= 0b10001000;
            // cpu->InPort[2] &= 0b10001011;

            // Check which key was pressed using the event data.
            switch (event.key.keysym.sym) {
                case SDLK_SPACE: // Fire
                    cpu->InPort[0] |= (1 << 4);
                    cpu->InPort[1] |= (1 << 4);
                    // cpu->InPort[2] |= (1 << 4); // P2 fire
                    break;

            
                case SDLK_LEFT: // Left
                    cpu->InPort[0] |= (1 << 5);
                    cpu->InPort[1] |= (1 << 5);
                    // cpu->InPort[2] |= (1 << 5); // P2 left
                    break;

          
                case SDLK_RIGHT: // Right
                    cpu->InPort[0] |= (1 << 6);
                    cpu->InPort[1] |= (1 << 6);
                    // cpu->InPort[2] |= (1 << 6); // P2 right
                    break;

                case SDLK_c: // Credit
                    cpu->InPort[1] |= (1 << 0);
                    break;

                case SDLK_1: // 1P Start
                    cpu->InPort[1] |= (1 << 2);
                    break;

                case SDLK_2: // 2P Start
                    cpu->InPort[1] |= (1 << 1);
                    break;

                case SDLK_DELETE: // Tilt
                    cpu->InPort[2] |= (1 << 2);
                    break;

                // Player 2 controls (if applicable)
                case SDLK_w: // P2 Fire
                    cpu->InPort[2] |= (1 << 4);
                    break;

                case SDLK_a: // P2 Left
                    cpu->InPort[2] |= (1 << 5);
                    break;

                case SDLK_d: // P2 Right
                    cpu->InPort[2] |= (1 << 6);
                    break;
               

                default:
                    break;
            }
        }

        if (event.type == SDL_KEYUP) {
            // Use keysym.sym for keyup events.
            switch (event.key.keysym.sym) {
                case SDLK_SPACE: // Fire released
                    // Clear Fire bit (bit 4) in all relevant ports.
                    cpu->InPort[0] &= ~(1 << 4);
                    cpu->InPort[1] &= ~(1 << 4);
                    // cpu->InPort[2] &= ~(1 << 4); // P2 fire
                    break;

                case SDLK_LEFT: // Left released
                    // Clear Left bit (bit 5) in all relevant ports.
                    cpu->InPort[0] &= ~(1 << 5);
                    cpu->InPort[1] &= ~(1 << 5);
                    // cpu->InPort[2] &= ~(1 << 5); // P2 left
                    break;

                case SDLK_RIGHT: // Right released
                    // Clear Right bit (bit 6) in all relevant ports.
                    cpu->InPort[0] &= ~(1 << 6);
                    cpu->InPort[1] &= ~(1 << 6);
                    // cpu->InPort[2] &= ~(1 << 6); // P2 right
                    break;

                case SDLK_c: // Credit released (only on Port 1)
                    cpu->InPort[1] &= ~(1 << 0);
                    break;

                case SDLK_1: // 1P Start released (assuming you use SDLK_1)
                    cpu->InPort[1] &= ~(1 << 2);
                    break;

                case SDLK_2: // 2P Start released
                    cpu->InPort[1] &= ~(1 << 1);
                    break;

                case SDLK_DELETE: // Tilt released (Port 2)
                    cpu->InPort[2] &= ~(1 << 2);
                    break;
                
                // Player 2 controls (if applicable)
                case SDLK_w: // P2 Fire
                    cpu->InPort[2] &= ~(1 << 4);
                    break;
                case SDLK_a: // P2 Left
                    cpu->InPort[2] &= ~(1 << 5);
                    break;
                case SDLK_d: // P2 Right
                    cpu->InPort[2] &= ~(1 << 6);
                    break;

                default:
                    break;
            }
        }
    
    }
}

void IO_device::output(I8080 *cpu){

    if ((cpu->OutPort[3] & 0x01) != 0) { // Check if bit 0 of OutPort is set
        if ((ch_Sound & (1 << 0)) == 0){
            playSound(0x00); // Play UFOSound
        }
        ch_Sound |= 0x01;
    }else {
        ch_Sound &= ~(1 << 0); // Clear the sound flag if bit 0 is not set
    }

    if ((cpu->OutPort[3] & 0x02) != 0) { // Check if bit 1 of OutPort is set
        if ((ch_Sound & (1 << 1)) == 0){
            playSound(0x01); // Play Shoot sound
        }
        ch_Sound |= 0x02;
    } else {
        ch_Sound &= ~(1 << 1); // Clear the sound flag if bit 1 is not set
    }

    if ((cpu->OutPort[3] & 0x04) != 0) { // Check if bit 2 of OutPort is set
        if ((ch_Sound & (1 << 2)) == 0){
            playSound(0x02); // Play PlayerDeath sound
        }
        ch_Sound |= 0x04;
    }
    else {
        ch_Sound &= ~(1 << 2); // Clear the sound flag if bit 2 is not set
    }

    if ((cpu->OutPort[3] & 0x08) != 0) { // Check if bit 3 of OutPort is set
        if ((ch_Sound & (1 << 3)) == 0){
            playSound(0x03); // Play InvaderDeath sound
        }
        ch_Sound |= 0x08;
    } else {
        ch_Sound &= ~(1 << 3); // Clear the sound flag if bit 3 is not set
    }

    if ((cpu->OutPort[5] & 0x01) != 0) { // Check if bit 4 of OutPort is set
        if ((ch_Sound & (1 << 4)) == 0){
            playSound(0x04); // Play Fleet1 sound
        }
        ch_Sound |= 0x10;
    }
    else {
        ch_Sound &= ~(1 << 4); // Clear the sound flag if bit 4 is not set
    }

    if ((cpu->OutPort[5] & 0x02) != 0) { // Check if bit 5 of OutPort is set
        if ((ch_Sound & (1 << 5)) == 0){
            playSound(0x05); // Play Fleet2 sound
        }
        ch_Sound |= 0x20;
    }
    else {
        ch_Sound &= ~(1 << 5); // Clear the sound flag if bit 5 is not set
    }


    if ((cpu->OutPort[5] & 0x04) != 0) { // Check if bit 6 of OutPort is set
        if ((ch_Sound & (1 << 6)) == 0){
            playSound(0x06); // Play Fleet3 sound
        }
        ch_Sound |= 0x40;
    }
    else {        
        ch_Sound &= ~(1 << 6); // Clear the sound flag if bit 6 is not set
    }


    if ((cpu->OutPort[5] & 0x08) != 0) { // Check if bit 7 of OutPort is set
        if ((ch_Sound & (1 << 7)) == 0){
            playSound(0x07); // Play Fleet4 sound
        }
        ch_Sound |= 0x80;
    }
    else {
        ch_Sound &= ~(1 << 7); // Clear the sound flag if bit 7 is not set
    }

}

void IO_device::playSound(uint8_t id) {
    switch (id) {
        case 0x00: // Example: Play UFOSound sound
            Mix_PlayChannel(0, UFOSound, 0); 
            std::cout << "UFOSound sound played\n";
            break;
        case 0x01: // Example: Play Shoot sound
            Mix_PlayChannel(0, Shoot, 0);
            std::cout << "Shoot sound played\n";
            break;
        case 0x02: // Example: Play PlayerDeath sound
            Mix_PlayChannel(0, PlayerDeath, 0);
            break;
        case 0x03: // Example: Play InvaderDeath sound
            Mix_PlayChannel(0, InvaderDeath, 0);
            std::cout << "InvaderDeath sound played\n";
            break;
        case 0x04: // Example: Play Fleet1 sound
            Mix_PlayChannel(0, Fleet1, 0);
            break;
        case 0x05: // Example: Play Fleet2 sound
            Mix_PlayChannel(0, Fleet2, 0);
            break;
        case 0x06: // Example: Play Fleet3 sound
            Mix_PlayChannel(0, Fleet3, 0);
            break;
        case 0x07: // Example: Play Fleet4 sound
            Mix_PlayChannel(0, Fleet4, 0);
            break;
        default:
            break;
    }
}