#include <iostream>
#include <fstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "chip8.h"

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 320

bool load_rom(chip8 &vm, const std::string &path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Failed to open file path: " << path << std::endl;
        return false;
    }
    std::streamsize size = file.tellg();
    if (size > (4096 - 0x200)) {
        std::cerr << "ROM size is over 4KB! Aborting" << std::endl;
        return false;
    }
    file.seekg(0, std::ios::beg);
    if (!file.read(reinterpret_cast<char *>(&vm.main_memory[0x200]), size)) {
        std::cerr << " Failed to load ROM from path: " << path << std::endl;
        return false;
    }
    return true;
}

void setup() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL Init failed: %s", SDL_GetError());
        abort();
    }
    SDL_Window *window = SDL_CreateWindow("PicklEmul", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer setup failed: %s", SDL_GetError());
        abort();
    }
    SDL_SetDefaultTextureScaleMode(renderer, SDL_SCALEMODE_NEAREST);
    SDL_SetRenderLogicalPresentation(renderer, 64, 32, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}


int main() {
    return 0;
}
