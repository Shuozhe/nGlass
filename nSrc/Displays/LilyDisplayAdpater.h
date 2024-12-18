#pragma once
#include "Display.h"
#include <core/lv_obj.h>

class LilyDisplayAdpater: public Display {
private:
  lv_obj_t *_window;
  int dmaBufferIndex = 0;

public:
  LilyDisplayAdpater(lv_obj_t *window);

  void drawJpg(void* data);

  void fillScreen(uint16_t color);
  void drawChannel(int channelIndex);
  void drawTuningText();
  void drawFPS(int fps);
};
