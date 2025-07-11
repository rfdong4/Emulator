#pragma once

#include <SDL2/SDL.h>

#include <cstdint>
#include <tuple>
// Controller Button Masks (Game Input Gpu)
#define CONTROLLER_A_MASK ((uint8_t)0x80)
#define CONTROLLER_B_MASK ((uint8_t)0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t)0x20)
#define CONTROLLER_START_MASK ((uint8_t)0x10)
#define CONTROLLER_UP_MASK ((uint8_t)0x08)
#define CONTROLLER_DOWN_MASK ((uint8_t)0x04)
#define CONTROLLER_LEFT_MASK ((uint8_t)0x02)
#define CONTROLLER_RIGHT_MASK ((uint8_t)0x01)

// Display constants
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 120
#define VRAM_START 0x3400
#define VRAM_END 0x6fff
#define WINDOW_SCALE 4     // Scale factor for the window size
#define FRAME_INTERVAL 16  // Render every 16ms (approximately 60 FPS)

class BananaConsole;  // Forward declaration

class BananaGpu {
 private:
  BananaConsole *console;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *texture;
  uint32_t last_render_time;
  uint8_t frame_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT * 3];

  // Helper methods
  uint16_t get_pixel_address(int width, int height) const;
  std::tuple<uint8_t, uint8_t, uint8_t> decode_pixel(uint8_t pixel) const;
  bool init_sdl();
  void cleanup();
  bool should_render() const;

 public:
  BananaGpu(BananaConsole *console);
  ~BananaGpu();

  // Main rendering method called after each CPU loop iteration
  void render();
};
