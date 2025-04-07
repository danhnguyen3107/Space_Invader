#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "include/I8080.h"
#include "include/IO_device.h"
#include <cassert>
#include <bitset>
#include <unistd.h>
#include <limits.h>
#include "include/Platform.h"
#include <iomanip>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Sprite dimensions and scale for bit drawing (Space Invader example)
const int SPRITE_WIDTH = 16;
const int SPRITE_HEIGHT = 16;
const int PIXEL_SIZE = 10;

// Path Roms
const char* ROM_PATH_H = "./src/Rom/invaders.h";
const char* ROM_PATH_G = "./src/Rom/invaders.g";
const char* ROM_PATH_F = "./src/Rom/invaders.f";
const char* ROM_PATH_E = "./src/Rom/invaders.e";

uint16_t spaceInvader[SPRITE_HEIGHT] = {
    0b0000011001100000,
    0b0000111111110000,
    0b0001111111111000,
    0b0011111111111100,
    0b0111111111111110,
    0b1111111111111111,
    0b1101111111110111,
    0b0001111111111000,
    0b0000011111100000,
    0b0000001111000000,
    0b0000011011000000,
    0b0000010011000000,
    0b0000000000000000,
    0b0000010011000000,
    0b0000110111100000,
    0b0001100011110000
};

// Function to draw the sprite based on the bit mask.
void drawSprite(SDL_Renderer* renderer, int posX, int posY, SDL_Color color) {

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int row = 0; row < SPRITE_HEIGHT; row++) {
        for (int col = 0; col < SPRITE_WIDTH; col++) {
            if (spaceInvader[row] & (1 << (SPRITE_WIDTH - 1 - col))) {
                SDL_Rect rect = { posX + col * PIXEL_SIZE, posY + row * PIXEL_SIZE, PIXEL_SIZE, PIXEL_SIZE };
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}


int test(){
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << "\n";
        return 1;
    }

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("Space Invader with Sound", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << "\n";
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << "\n";
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load sound effects for left, right, and space key presses
    Mix_Chunk* leftSound = Mix_LoadWAV("Sound/Sounds_explosion.wav");
    Mix_Chunk* rightSound = Mix_LoadWAV("Sound/Sounds_invaderkilled.wav");
    Mix_Chunk* spaceSound = Mix_LoadWAV("Sound/Sounds_shoot.wav");
    if (!leftSound || !rightSound || !spaceSound) {
        std::cerr << "Failed to load one or more sound effects! SDL_mixer Error: " << Mix_GetError() << "\n";
        Mix_CloseAudio();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    // Sprite color (can change with keys too if desired)
    SDL_Color spriteColor = {255, 255, 255, 255};

    // Main loop
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                // Trigger sound effects on left, right, or space key press
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        Mix_PlayChannel(-1, leftSound, 0);
                        break;
                    case SDLK_RIGHT:
                        Mix_PlayChannel(-1, rightSound, 0);
                        break;
                    case SDLK_SPACE:
                        Mix_PlayChannel(-1, spaceSound, 0);
                        break;
                    // Optionally update sprite color with other keys:
                    case SDLK_r:
                        spriteColor = {255, 0, 0, 255};
                        break;
                    case SDLK_g:
                        spriteColor = {0, 255, 0, 255};
                        break;
                    case SDLK_b:
                        spriteColor = {0, 0, 255, 255};
                        break;
                    case SDLK_w:
                        spriteColor = {255, 255, 255, 255};
                        break;
                    case SDLK_k:
                        spriteColor = {0, 0, 0, 255};
                        break;
                    default:
                        break;
                }
            }
        }

        // Clear screen (black)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Center the sprite on the screen
        int posX = (SCREEN_WIDTH - SPRITE_WIDTH * PIXEL_SIZE) / 2;
        int posY = (SCREEN_HEIGHT - SPRITE_HEIGHT * PIXEL_SIZE) / 2;
        drawSprite(renderer, posX, posY, spriteColor);

        // Update screen
        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS
    }

    // Cleanup sound effects
    Mix_FreeChunk(leftSound);
    Mix_FreeChunk(rightSound);
    Mix_FreeChunk(spaceSound);
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

int test_unit() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << "\n";
        return -1;
    }
    
    // Create a dummy window (required by some SDL functionality)
    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Create our CPU and I/O device objects.
    I8080 cpu;
    IO_device io;

    // ----- Test Input: Simulate keydown and keyup for "Fire" (Space) -----
    // Simulate keydown for SPACE.
    SDL_Event event;
    event.type = SDL_KEYDOWN;
    event.key.keysym.sym = SDLK_SPACE;
    SDL_PushEvent(&event);

    // Call handle_input to process the event.
    io.handle_input(&cpu);

    // Check that the Fire bit (bit 4) is set on ports 0, 1, and 2.
    assert((cpu.InPort[0] & (1 << 4)) != 0);
    assert((cpu.InPort[1] & (1 << 4)) != 0);
    assert((cpu.InPort[2] & (1 << 4)) != 0);
    std::cout << "Keydown SPACE: Fire bits correctly set.\n";

    // Simulate keyup for SPACE.
    event.type = SDL_KEYUP;
    event.key.keysym.sym = SDLK_SPACE;
    SDL_PushEvent(&event);

    // Process keyup event.
    io.handle_input(&cpu);

    // Check that the Fire bit (bit 4) is cleared on all ports.
    assert((cpu.InPort[0] & (1 << 4)) == 0);
    assert((cpu.InPort[1] & (1 << 4)) == 0);
    assert((cpu.InPort[2] & (1 << 4)) == 0);
    std::cout << "Keyup SPACE: Fire bits correctly cleared.\n";


    
    // ----- Test Output: Simulate setting OutPort bits to play sounds -----
    // Set OutPort[3] bit 0 to trigger the UFO sound.
    
    cpu.OutPort[3] = 0x01;  // Bit 0 set.
    io.output(&cpu);        // Should call playSound(0x00).

    // Set OutPort[3] bit 1 to trigger the Shoot sound.
    cpu.OutPort[3] = 0x02;  // Bit 1 set.
    io.output(&cpu);        // Should call playSound(0x01).

    // Similarly, test other output bits if desired...
    // For instance:
    cpu.OutPort[5] = 0x01;  // Bit 0 of OutPort[5] for Fleet1 sound.
    io.output(&cpu);        // Should call playSound(0x04).

    std::cout << "Output test: Sound triggers processed.\n";

    // Cleanup SDL.
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "All tests passed.\n";


    return 0;
}


void process(I8080* cpu, IO_device* io, uint8_t* checkSound) {
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

int test_keyboard() {
    // Initialize SDL (video and audio).
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << "\n";
        return -1;
    }
    
    // Create a dummy window (required by some SDL functionality)
    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED,
                                            SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return -1;
    }

    // Create our CPU and I/O device objects.
    I8080 cpu;
    IO_device io;
    
    // Main loop.
    bool running = true;

    uint8_t checkSound = 0x00;

    uint32_t lastFrameTime = SDL_GetTicks();
    uint32_t currentTime = SDL_GetTicks();

    while (running && !cpu.halt) {
        // Handle input from the keyboard.
        io.handle_input(&cpu);
        
    
        // io.output(&cpu);
        process(&cpu, &io, &checkSound);

        // For testing, print the status of the input ports.
        std::cout << "checkSound: " << std::bitset<8>(checkSound) << std::endl;
        std::cout 
                    << "InPort[0]: " << std::bitset<8>(cpu.InPort[0])
                    << "  InPort[1]: " << std::bitset<8>(cpu.InPort[1])
                    << "  InPort[2]: " << std::bitset<8>(cpu.InPort[2]) << "\r";
        //         //   << " OutPort[3]: " << static_cast<int>(cpu.OutPort[3])
        //         //   << " OutPort[5]: " << static_cast<int>(cpu.OutPort[5]) << "\r";
        std::cout.flush();
        
        // Delay to limit CPU usage (approximately 60 FPS).
        SDL_Delay(16);
        
        // You can exit by closing the window or by a key event that sets cpu.halt.
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}


void loadROM(I8080* cpu) {
    cpu->loadROM(ROM_PATH_H, INVADER_START_ADDRESS_H);
    cpu->loadROM(ROM_PATH_G, INVADER_START_ADDRESS_G);
    cpu->loadROM(ROM_PATH_F, INVADER_START_ADDRESS_F);
    cpu->loadROM(ROM_PATH_E, INVADER_START_ADDRESS_E);
    // cpu->loadROM("./src/Rom/invaders", 0x0000);

    std::cout << "ROM loaded successfully." << std::endl;

    // print the loaded ROM for debugging
    for (int i = 0; i < 0x2000; i++) {
        if (i % 16 == 0) {
            std::cout << std::endl << std::hex << i << ": ";
        }
        std::cout << std::hex << static_cast<int>(cpu->memory[i]) << " ";
    }
    std::cout << std::endl;

}

// output rom from cpu memory to text file
void outputMemoryToFile(const I8080 &cpu, const std::string &filename) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error opening output file: " << filename << std::endl;
        return;
    }

    outFile << "Memory Dump:\n\n";

    // Section 1: 8K ROM (0000-1FFF)
    outFile << "8K ROM (0000-1FFF):\n";
    for (uint32_t addr = 0x0000; addr <= 0x1FFF; addr += 16) {
        outFile << std::setw(4) << std::setfill('0') << std::hex << addr << ": ";
        for (uint32_t j = 0; j < 16 && (addr + j) <= 0x1FFF; ++j) {
            outFile << std::setw(2) << std::setfill('0') 
                    << std::hex << static_cast<int>(cpu.memory[addr + j]) << " ";
        }
        outFile << "\n";
    }
    outFile << "\n";

    // Section 2: 1K RAM (2000-23FF)
    outFile << "1K RAM (2000-23FF):\n";
    for (uint32_t addr = 0x2000; addr <= 0x23FF; addr += 16) {
        outFile << std::setw(4) << std::setfill('0') << std::hex << addr << ": ";
        for (uint32_t j = 0; j < 16 && (addr + j) <= 0x23FF; ++j) {
            outFile << std::setw(2) << std::setfill('0') 
                    << std::hex << static_cast<int>(cpu.memory[addr + j]) << " ";
        }
        outFile << "\n";
    }
    outFile << "\n";

    // Section 3: 7K Video RAM (2400-3FFF)
    outFile << "7K Video RAM (2400-3FFF):\n";
    for (uint32_t addr = 0x2400; addr <= 0x3FFF; addr += 16) {
        outFile << std::setw(4) << std::setfill('0') << std::hex << addr << ": ";
        for (uint32_t j = 0; j < 16 && (addr + j) <= 0x3FFF; ++j) {
            outFile << std::setw(2) << std::setfill('0') 
                    << std::hex << static_cast<int>(cpu.memory[addr + j]) << " ";
        }
        outFile << "\n";
    }
    outFile << "\n";

    // Section 4: RAM Mirror (4000-MEMORY_SIZE-1)
    outFile << "RAM Mirror (4000-" 
            << std::setw(4) << std::setfill('0') << std::hex << (MEMORY_SIZE - 1)
            << "):\n";
    for (uint32_t addr = 0x4000; addr < MEMORY_SIZE; addr += 16) {
        outFile << std::setw(4) << std::setfill('0') << std::hex << addr << ": ";
        for (uint32_t j = 0; j < 16 && (addr + j) < MEMORY_SIZE; ++j) {
            outFile << std::setw(2) << std::setfill('0') 
                    << std::hex << static_cast<int>(cpu.memory[addr + j]) << " ";
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Memory output to " << filename << " completed." << std::endl;
}

void printWorkingDirectory() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working directory: " << cwd << std::endl;
    } else {
        perror("getcwd() error");
    }
}

int main(int argc, char* argv[]) {
    // printWorkingDirectory();
    I8080 cpu;
    IO_device io;
    Platform platform;

    loadROM(&cpu);

    platform.Initialize(&cpu, &io);
    platform.EmulatorCycle();
    
    outputMemoryToFile(cpu, "output.txt");

    return 0;
}





















