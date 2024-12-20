#pragma once

#include <Arduino.h>

class Display {
  public:
  virtual void drawJpg(void* data) = 0;
  virtual void fillScreen(uint16_t color) = 0;
  virtual void drawChannel(int channelIndex) = 0;
  virtual void drawTuningText() = 0;
  virtual void drawFPS(int fps) = 0;

  static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }
};

namespace DisplayColors {
  static const uint16_t BLACK = Display::color565(0, 0, 0);
};