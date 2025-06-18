#include "BananaConsole.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

BananaConsole::BananaConsole(const std::string &filename)
    : filename_(filename), memory(this, filename), gpu(this) {}

void BananaConsole::run() {
  BananaCpu cpu(&memory, &gpu);

  // Initial setup loop
  while (cpu.pc != 0x0000) {
    cpu.step();
  }

  // Main game loop
  uint16_t start = cpu.memory->address_to_loop();
  uint32_t lastTime = SDL_GetTicks();
  const uint32_t STEP_INTERVAL = 16;  // Run CPU steps at ~60Hz

  while (true) {
    uint32_t current_time = SDL_GetTicks();

    if ((current_time - lastTime) >= STEP_INTERVAL) {
      // Render first

      // Only step CPU after frame is rendered
      cpu.pc = start;
      while (cpu.pc != 0x0000) {
        cpu.step();
      }
      gpu.render();
      lastTime = current_time;
    } else {
      SDL_Delay(1);  // Yield CPU time when waiting
    }
  }
}
