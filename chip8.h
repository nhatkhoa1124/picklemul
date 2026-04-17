#pragma once
#include <algorithm>
#include <cstdint>
#include <random>

struct chip8 {
    uint8_t main_memory[4096] = {0};
    uint16_t stack_memory[16] = {0};
    uint8_t display_buffer[2048] = {0};
    uint8_t registers[16] = {0};
    uint16_t index_register = 0;
    uint16_t program_counter = 0x200;
    uint8_t stack_pointer = 0;
    uint8_t timers[2] = {0};
    uint8_t keypads[16] = {0};

    int rand;


    ~chip8() = default;

    void init();

    void reset();

    uint16_t fetch();

    void decode(uint16_t opcode);

    void exec();

    void update_timers();

    void set_key(uint8_t key_index, bool is_pressed);
};

inline void chip8::init() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(0, 255);
    rand = distr(gen);
}

inline void chip8::reset() {
    std::ranges::fill(main_memory, 0);
    std::ranges::fill(stack_memory, 0);
    std::ranges::fill(display_buffer, 0);
    std::ranges::fill(general_registers, 0);
    std::ranges::fill(keypads, 0);
    program_counter = 0x200;
    index_register = 0;
    stack_pointer = 0;
    timers[0] = 0;
    timers[1] = 1;
}

inline uint16_t chip8::fetch() {
    uint16_t first_byte = main_memory[program_counter];
    uint16_t second_byte = main_memory[program_counter + 1];
    uint16_t instruction = (first_byte) << 8 | second_byte;
    program_counter += 2;
    return instruction;
}

inline void chip8::decode(uint16_t opcode) {
    const uint16_t first = 0xF000 & opcode;
    uint16_t x = (0x0F00 & opcode) >> 8;
    uint16_t y = (0x00F0 & opcode) >> 4;
    uint16_t n = 0x000F & opcode;
    uint16_t nn = 0x00FF & opcode;
    uint16_t nnn = 0x0FFF & opcode;

    switch (first) {
        case 0x0000:
            switch (nn) {
                case 0xE0:
                    std::ranges::fill(display_buffer, 0);
                    break;
                case 0xEE:
                    stack_pointer--;
                    program_counter = stack_memory[stack_pointer];
                    break;
            }
            break;
        case 0x1000:
            program_counter = nnn;
            break;
        case 0x2000:
            stack_memory[stack_pointer] = program_counter;
            stack_pointer++;
            program_counter = nnn;
            break;
        case 0x3000:
            if (registers[x] == nn)
                program_counter += 2;
            break;
        case 0x4000:
            if (registers[x] != nn)
                program_counter += 2;
            break;
        case 0x5000:
            if (registers[x] == registers[y])
                program_counter += 2;
            break;
        case 0x6000:
            registers[x] = nn;
            break;
        case 0x7000:
            registers[x] += nn;
            break;
        case 0x8000:
            switch (n) {
                case 0:
                    registers[x] = registers[y];
                    break;
                case 1:
                    registers[x] |= registers[y];
                    break;
                case 2:
                    registers[x] &= registers[y];
                    break;
                case 3:
                    registers[x] ^= registers[y];
                    break;
                case 4: {
                    auto sum = registers[x] + registers[y];
                    if (sum > 255)
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] = sum & 0xFF;
                    break;
                }
                case 5:
                    if (registers[x] > registers[y])
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] -= registers[y];
                    break;
                case 6:
                    if ((0x1 & registers[x]) == 1)
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] /= 2;
                    break;
                case 7:
                    if (registers[y] > registers[x])
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] = registers[y] - registers[x];
                    break;
                case 0xE:
                    if ((registers[x] >> 7) == 1)
                        registers[0xF] = 1;
                    else
                        registers[0xF] = 0;
                    registers[x] *= 2;
                    break;
            }
            break;
        case 0x9000:
            if (registers[x] != registers[y])
                program_counter += 2;
            break;
        case 0xA000:
            index_register = nnn;
            break;
        case 0xB000:
            program_counter = nnn + registers[0];
            break;
        case 0xC000: {
            registers[x] = nn & rand;
            break;
        }
        case 0xD000:
            break;
        case 0xE000:
            switch (nn) {
                case 0x9E:
                    if (keypads[registers[x]] == 1)
                        program_counter += 2;
                    break;
                case 0xA1:
                    if (keypads[registers[x]] == 0)
                        program_counter += 2;
                    break;
            }
            break;
        case 0xF000:
            switch (nn) {
                case 0x07:
                    registers[x] = timers[0];
                    break;
                case 0x0A: {
                    bool is_pressed = false;
                    for (size_t i = 0; i < sizeof(keypads); i++) {
                        if (keypads[i] == 1) {
                            registers[x] = i;
                            is_pressed = true;
                            break;
                        }
                    }
                    if (!is_pressed)
                        program_counter -= 2;
                    break;
                }
                case 0x15:
                    timers[0] = registers[x]; // Set delay timer
                    break;
                case 0x18:
                    timers[1] = registers[x]; // Set sound timer
                    break;
                case 0x1E:
                    index_register += registers[x];
                    break;
                case 0x29:
                    index_register = registers[x] * 5;
                    break;
                case 0x33:
                    main_memory[index_register] = registers[x] / 100;
                    main_memory[index_register + 1] = (registers[x] / 10) % 10;
                    main_memory[index_register + 2] = registers[x] % 10;
                    break;
                case 0x55:
                    for (size_t i = 0; i <= x; i++) {
                        main_memory[index_register + i] = registers[i];
                    }
                    break;
                case 0x65:
                    for (size_t i = 0; i <= x; i++) {
                        registers[i] = main_memory[index_register + i];
                    }
                    break;
            }
            break;
        default:
            break;
    }
}

inline void chip8::exec() {
}

inline void chip8::update_timers() {
}

inline void chip8::set_key(uint8_t key_index, bool is_pressed) {
    if (key_index < 16 && is_pressed) {
        keypads[key_index] = is_pressed;
    }
}
