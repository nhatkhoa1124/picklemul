# PicklEmul

CHIP-8 emulator project with a shared C++ core and two frontends:
- Desktop: SDL3 (`src/desktop/main.cpp`)
- Mobile app: React Native + Skia (`PicklemulMobile/`)

## Technical summary

- Core VM in `src/core/chip8.h`:
    - 4 KB memory, 16 registers, 16-level stack, 64x32 display buffer, 16-key keypad
    - fontset preload, fetch/decode/execute loop, timers, key input API
    - opcode decode implemented through CHIP-8 instruction groups (`0x0***` ... `0xF***`)
- Desktop runtime (`src/desktop/main.cpp`):
    - SDL3 renderer with logical resolution `64x32`, nearest scaling, letterboxing
    - CPU pacing: `1200` cycles/sec at `60` FPS (`20` cycles/frame)
    - ROM file loading with size guard (`<= 4096 - 0x200`)
- Android mobile runtime:
    - Native bridge: JS -> Java module -> JNI -> shared C++ emulator (`chip8_bridge`)
    - Synchronous React Native native module methods for init/exec/load/timers/keys/framebuffer
    - Skia framebuffer rendering and on-screen 16-key keypad in `PicklemulMobile/App.tsx`
    - Time-based stepping (target `1200` CPU Hz, `60` timer Hz)

## ROMs in repository

- Desktop assets (`assets/`): `RPS.ch8`, `snake.ch8`, `spacejam.ch8`, `spaceracer.ch8`
- Mobile embedded ROM arrays (`PicklemulMobile/src/roms.ts`):
    - IBM Logo
    - Snake
    - Space Racer
    - Space Jam
    - RPS

## Key mapping (desktop and mobile UI)

| CHIP-8 | Key |
|---|---|
| `1 2 3 C` | `1 2 3 4` |
| `4 5 6 D` | `Q W E R` |
| `7 8 9 E` | `A S D F` |
| `A 0 B F` | `Z X C V` |

## Build and run

### Desktop (CMake + SDL3)

Requirements:
- C++20 compiler
- CMake `>= 4.1`

Commands:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

Run `picklemul` from the build output directory.

### Mobile (React Native)

From `PicklemulMobile/`:

```bash
npm install
npm run android
```

`npm run ios` is available in scripts; Android native bridge implementation is present in this repository.
