#include "BananaGpu.h"

#include <cstdio>
#include <iostream>

#include "BananaConsole.h"
#include "BananaMemory.h"

BananaGpu::BananaGpu(BananaConsole *console)
    : console(console), last_render_time(0) {
  if (!init_sdl()) {
    std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
    exit(1);
  }
}

BananaGpu::~BananaGpu() { cleanup(); }

bool BananaGpu::init_sdl() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    return false;
  }

  window =
      SDL_CreateWindow("Banana Console", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, DISPLAY_WIDTH * WINDOW_SCALE,
                       DISPLAY_HEIGHT * WINDOW_SCALE, SDL_WINDOW_SHOWN);

  if (!window) {
    return false;
  }

  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    return false;
  }

  // Create texture with the optimal format for your platform
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                              SDL_TEXTUREACCESS_STREAMING, DISPLAY_WIDTH,
                              DISPLAY_HEIGHT);

  if (!texture) {
    return false;
  }

  // Set scaling quality
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

  return true;
}

void BananaGpu::cleanup() {
  if (texture) {
    SDL_DestroyTexture(texture);
  }
  if (renderer) {
    SDL_DestroyRenderer(renderer);
  }
  if (window) {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();
}

uint16_t BananaGpu::get_pixel_address(int width, int height) const {
  int pixel_index = width + (height * DISPLAY_WIDTH);
  int pixel_offset = 1 * pixel_index;  // Each pixel is 1 byte
  return VRAM_START + pixel_offset;
}

std::tuple<uint8_t, uint8_t, uint8_t> BananaGpu::decode_pixel(
    uint8_t pixel) const {
  // Bits 5-4 are red (2 bits)
  // Bits 3-2 are green (2 bits)
  // Bits 1-0 are blue (2 bits)
  uint8_t r =
      ((pixel >> 4) & 0x03) * 85;  // Shift right by 4 to get to bits 5-4
  uint8_t g =
      ((pixel >> 2) & 0x03) * 85;   // Shift right by 2 to get to bits 3-2
  uint8_t b = (pixel & 0x03) * 85;  // Bits 1-0 are already in position
  return std::make_tuple(r, g, b);
}

bool BananaGpu::should_render() const {
  uint32_t current_time = SDL_GetTicks();
  // Only render if enough time has passed since the last render
  if ((current_time - last_render_time) >= FRAME_INTERVAL) {
    return true;
  }
  // If not enough time has passed, add a small delay to prevent CPU spinning
  // SDL_Delay(1);
  return false;
}

void BananaGpu::render() {
  if (!should_render()) {
    return;
  }

  // Handle SDL events
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      exit(0);
    }
    if (event.type == SDL_KEYDOWN) {
      SDL_Keycode key = event.key.keysym.sym;
      if (key == SDLK_a) {
        console->input |= CONTROLLER_LEFT_MASK;
      }
      if (key == SDLK_w) {
        console->input |= CONTROLLER_UP_MASK;
      }
      if (key == SDLK_s) {
        console->input |= CONTROLLER_DOWN_MASK;
      }
      if (key == SDLK_d) {
        console->input |= CONTROLLER_RIGHT_MASK;
      }
      if (key == SDLK_g) {
        console->input |= CONTROLLER_SELECT_MASK;
      }
      if (key == SDLK_h) {
        console->input |= CONTROLLER_START_MASK;
      }
      if (key == SDLK_k) {
        console->input |= CONTROLLER_A_MASK;
      }
      if (key == SDLK_l) {
        console->input |= CONTROLLER_B_MASK;
      }
    }
    if (event.type == SDL_KEYUP) {
      SDL_Keycode key = event.key.keysym.sym;
      if (key == SDLK_a) {
        console->input &= ~CONTROLLER_LEFT_MASK;
      }
      if (key == SDLK_w) {
        console->input &= ~CONTROLLER_UP_MASK;
      }
      if (key == SDLK_s) {
        console->input &= ~CONTROLLER_DOWN_MASK;
      }
      if (key == SDLK_d) {
        console->input &= ~CONTROLLER_RIGHT_MASK;
      }
      if (key == SDLK_g) {
        console->input &= ~CONTROLLER_SELECT_MASK;
      }
      if (key == SDLK_h) {
        console->input &= ~CONTROLLER_START_MASK;
      }
      if (key == SDLK_k) {
        console->input &= ~CONTROLLER_A_MASK;
      }
      if (key == SDLK_l) {
        console->input &= ~CONTROLLER_B_MASK;
      }
    }
  }

  // Lock texture for direct access
  void *pixels;
  int pitch;
  SDL_LockTexture(texture, NULL, &pixels, &pitch);

  // Update all pixels
  uint8_t *pixel_data = static_cast<uint8_t *>(pixels);
  for (int y = 0; y < DISPLAY_HEIGHT; y++) {
    for (int x = 0; x < DISPLAY_WIDTH; x++) {
      uint16_t addr = get_pixel_address(x, y);
      uint8_t pixel = console->memory.l8_u(addr);

      // Calculate position in texture
      int pos = (y * pitch) + (x * 3);  // 3 bytes per pixel (RGB)

      // Decode the pixel
      std::tuple t = decode_pixel(pixel);
      pixel_data[pos] = std::get<0>(t);      // R
      pixel_data[pos + 1] = std::get<1>(t);  // G
      pixel_data[pos + 2] = std::get<2>(t);  // B
    }
  }

  // Unlock texture
  SDL_UnlockTexture(texture);

  // Clear and render
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  last_render_time = SDL_GetTicks();
}
