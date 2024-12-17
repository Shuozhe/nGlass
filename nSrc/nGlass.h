#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "VideoPlayer.h"
#include <LV_Helper.h>
#include <LilyGo_Wristband.h>
#include <WiFi.h>
#include "VideoSource/NetworkVideoSource.h"
#include "ChannelData/NetworkChannelData.h"
#include "Displays/LilyDisplayAdpater.h"

#define GLASS_VIEWABLE_WIDTH 126
#define GLASS_VIEWABLE_HEIGHT 100

#define WIFI_SSID "TEST"
#define WIFI_PASSWORD "TEST"

const char *FRAME_URL = "http://192.168.1.229:8123:8123/frame";
const char *CHANNEL_INFO_URL = "http://192.168.1.229:8123/channel_info";

class NGlass
{
private:
  LilyGo_Class lily;

  VideoPlayer *mVideoPlayer = NULL;

public:
  NGlass()
  {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    WiFi.setSleep(false);
    WiFi.setTxPower(WIFI_POWER_19_5dBm);
    Serial.println("");
    // disable WiFi power saving for speed
    Serial.println("WiFi connected");
    NetworkChannelData *channelData = new NetworkChannelData(CHANNEL_INFO_URL, FRAME_URL);
    NetworkVideoSource *videoSource = new NetworkVideoSource((NetworkChannelData *) channelData);
    Display *displayAdapter = new LilyDisplayAdpater(&lily);
    mVideoPlayer = new VideoPlayer(channelData, videoSource, displayAdapter);
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

    lily.setRotation(0);
    lily.setBrightness(0);

    beginLvglHelper(lily);

    // Set display background color to black
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), 0);

    // Create a display window object
    lv_obj_t *window = lv_obj_create(lv_scr_act());
    lv_obj_set_style_border_width(window, 3, 0);
    lv_obj_set_size(window, GLASS_VIEWABLE_WIDTH, GLASS_VIEWABLE_HEIGHT);
    lv_obj_align(window, LV_ALIGN_BOTTOM_MID, 0, 0);
  }

  void tick()
  {
    

    lily.update();
    lv_timer_handler();
    delay(1);
  }

private:
};
