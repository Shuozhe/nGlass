#pragma once

#include <Arduino.h>
#include <Wire.h>
#include <LilyGo_Wristband.h>
#include <LV_Helper.h>
#include <demos/widgets/lv_demo_widgets.h>

#define GLASS_VIEWABLE_WIDTH 126
#define GLASS_VIEWABLE_HEIGHT 126

class NGlass
{
private:
  LilyGo_Class lily;

public:
  NGlass()
  {
  }

  void init()
  {
    bool result = lily.begin();
    if (!result)
    {
      while (1)
      {
        Serial.println("The board model cannot be detected, please raise the Core Debug Level to an error");
        delay(1000);
      }
    }

    beginLvglHelper(lily);

    // Set display background color to black
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    // Create a display window object
    lv_obj_t *window = lv_obj_create(lv_scr_act());
    lv_obj_set_style_border_width(window, 0, 0);
    lv_obj_set_size(window, GLASS_VIEWABLE_WIDTH, GLASS_VIEWABLE_HEIGHT);
    lv_obj_align(window, LV_ALIGN_BOTTOM_MID, 0, 0);
  }

  void tick()
  {
    lv_demo_widgets();
    // lvgl task processing should be placed in the loop function
    lv_timer_handler();
    delay(2);
  }

private:
};
