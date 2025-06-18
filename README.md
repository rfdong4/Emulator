# Banana Emulator

Banana is a C++ emulator for a custom CPU, GPU, and memory system, designed to run binary `.slug` files. It features a simple architecture and uses SDL2 for graphics output. The project includes example games and a test suite for validation.

## Features
- Emulates a custom 16-bit CPU with 32 registers
- GPU with 128x120 display, controller input, and SDL2 rendering
- Memory management for ROM and RAM
- Runs binary `.slug` files
- Automated test suite

## Requirements
- C++17 compatible compiler
- [SDL2](https://www.libsdl.org/) development libraries
- CMake >= 3.22.1 (recommended) or GNU Make

## Build Instructions

### Using CMake
```bash
mkdir build
cd build
cmake ..
make
```
This will produce the `Banana` executable in the `build/` directory.

### Using Make (for formatting and hello_world example)
- `make format` — Format all source files with clang-format (Google style)
- `make verify-format` — Check formatting
- `make hello_world` — Build a simple hello_world example (not the emulator)

## Usage
Run the emulator with a `.slug` binary file:
```bash
./Banana <slug_file>
```
Example:
```bash
./Banana ../Games\ and\ tests/games/your_game.slug
```

## Testing
Automated tests are provided in the `tests/` directory. Each test has input and expected output files. Use the provided script to run tests:
```bash
cd tests
./run_test.sh ./hello_world1 ../build/Banana hws/hello_world1.slug
```
Repeat for other test cases as needed.

## Project Structure
- `src/` — Emulator source code (CPU, GPU, Memory, Console)
- `Games and tests/` — Example games and test ROMs
- `tests/` — Test scripts and expected outputs
- `build/` — Build artifacts (after compilation)


