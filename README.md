# PicklEmul

A custom-built CHIP-8 emulator written in C++20 and SDL3. This project is a cycle-accurate software interpreter that abstractly emulates the CPU, RAM, registers, and display of a 1970s CHIP-8 virtual machine.

## Features
* **Headless Core:** The `chip8` engine is decoupled from the UI, making it highly portable.
* **Modern Rendering:** Uses SDL3 with logical presentation scaling and nearest-neighbor texture filtering to keep the 64x32 grid perfectly sharp on modern displays.
* **Timers:** Accurate 60Hz decay for both sound and delay timers.

## Controls
The original CHIP-8 used a 16-key hex keypad (0-F). PicklEmul maps these inputs to the left side of a standard QWERTY keyboard:

| CHIP-8 Key | QWERTY Key |
| :--- | :--- |
| **1  2  3  C** | **1  2  3  4** |
| **4  5  6  D** | **Q  W  E  R** |
| **7  8  9  E** | **A  S  D  F** |
| **A  0  B  F** | **Z  X  C  V** |

## Requirements
* A C++20 compatible compiler (MSVC, GCC, or Clang)
* CMake 4.1 or higher
* Internet connection (for the initial CMake run to download SDL3)

## Building and Running
1. Clone the repository.
2. Place your CHIP-8 ROM files (e.g., `.ch8`) inside the `assets/` folder. Update the path in `filepaths.h` if necessary.
3. Generate the build files and compile using CMake:

    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build .
    ```

4. Run the executable. *(Note for Windows users: Ensure `SDL3.dll` is located in the same directory as `picklemul.exe` if CMake does not copy it automatically).*
