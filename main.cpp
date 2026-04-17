#include <iostream>
#include <fstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "chip8.h"

#define WINDOW_LENGTH = 640;
#define WINDOW_HEIGHT = 320;

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

int main() {
    return 0;
}
