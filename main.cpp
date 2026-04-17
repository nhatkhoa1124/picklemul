#include <iostream>
#include <fstream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "chip8.h"
#include "filepaths.h"

#define WINDOW_WIDTH  640
#define WINDOW_HEIGHT 320

SDL_Window *g_window = nullptr;
SDL_Renderer *g_renderer = nullptr;

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
    g_window = SDL_CreateWindow("PicklEmul", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    g_renderer = SDL_CreateRenderer(g_window, nullptr);
    if (!g_renderer) {
        SDL_Log("Renderer setup failed: %s", SDL_GetError());
        abort();
    }
    SDL_SetDefaultTextureScaleMode(g_renderer, SDL_SCALEMODE_NEAREST);
    SDL_SetRenderLogicalPresentation(g_renderer, 64, 32, SDL_LOGICAL_PRESENTATION_LETTERBOX);
}

void draw(SDL_Renderer *renderer, chip8 &vm) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (vm.display_buffer[x + (y * 64)]) {
                SDL_RenderPoint(renderer, static_cast<float>(x), static_cast<float>(y));
            }
        }
    }
    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[]) {
    chip8 emulator = {};
    emulator.init();
    if (!load_rom(emulator, RPS)) {
        return -1;
    }
    setup();

    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                quit = true;
            }
        }
        for (int i = 0; i < 10; i++) {
            emulator.exec();
        }
        emulator.update_timers();
        if (emulator.draw_flag) {
            draw(g_renderer, emulator);
            emulator.draw_flag = false;
        }
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);
    SDL_Quit();
    return 0;
}
