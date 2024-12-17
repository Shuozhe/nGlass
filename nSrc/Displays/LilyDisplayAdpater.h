#pragma once
#ifndef LED_MATRIX
#include "Display.h"
#include <LilyGo_Display.h>

class LilyDisplayAdpater: public Display {
private:
  LilyGo_Display *lily;
  uint16_t *dmaBuffer[2] = {NULL, NULL};
  int dmaBufferIndex = 0;
public:
  LilyDisplayAdpater(LilyGo_Display *lily);
  void drawPixels(int x, int y, int width, int height, uint16_t *pixels);
  void startWrite();
  void endWrite();
  int width();
  int height();
  void fillScreen(uint16_t color);
  void drawChannel(int channelIndex);
  void drawTuningText();
  void drawFPS(int fps);
  void drawErrorMessage(char *error);
};
#endif