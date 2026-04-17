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
            if (e.type == SDL_EVENT_KEY_DOWN || e.type == SDL_EVENT_KEY_UP) {
                bool is_pressed = (e.type == SDL_EVENT_KEY_DOWN);

                switch (e.key.key) {
                    case SDLK_1: emulator.set_key(0x1, is_pressed); break;
                    case SDLK_2: emulator.set_key(0x2, is_pressed); break;
                    case SDLK_3: emulator.set_key(0x3, is_pressed); break;
                    case SDLK_4: emulator.set_key(0xC, is_pressed); break;

                    case SDLK_Q: emulator.set_key(0x4, is_pressed); break;
                    case SDLK_W: emulator.set_key(0x5, is_pressed); break;
                    case SDLK_E: emulator.set_key(0x6, is_pressed); break;
                    case SDLK_R: emulator.set_key(0xD, is_pressed); break;

                    case SDLK_A: emulator.set_key(0x7, is_pressed); break;
                    case SDLK_S: emulator.set_key(0x8, is_pressed); break;
                    case SDLK_D: emulator.set_key(0x9, is_pressed); break;
                    case SDLK_F: emulator.set_key(0xE, is_pressed); break;

                    case SDLK_Z: emulator.set_key(0xA, is_pressed); break;
                    case SDLK_X: emulator.set_key(0x0, is_pressed); break;
                    case SDLK_C: emulator.set_key(0xB, is_pressed); break;
                    case SDLK_V: emulator.set_key(0xF, is_pressed); break;
                }
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
