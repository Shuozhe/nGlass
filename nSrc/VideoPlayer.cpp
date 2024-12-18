#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "VideoPlayer.h"
#include "ChannelData/ChannelData.h"
#include "VideoSource/VideoSource.h"
#include "Displays/Display.h"
#include <list>

void VideoPlayer::_framePlayerTask(void *param)
{
  VideoPlayer *player = (VideoPlayer *)param;
  player->framePlayerTask();
}

VideoPlayer::VideoPlayer(ChannelData *channelData, VideoSource *videoSource, Display *display)
: mChannelData(channelData), mVideoSource(videoSource), mDisplay(display), mState(VideoPlayerState::STOPPED)
{
}

void VideoPlayer::start()
{
  mVideoSource->start();
  // launch the frame player task
  xTaskCreatePinnedToCore(
      _framePlayerTask,
      "Frame Player",
      10000,
      this,
      1,
      NULL,
      0);
}

void VideoPlayer::setChannel(int channel)
{
  mChannelData->setChannel(channel);
  // set the audio sample to 0 - TODO - move this somewhere else?
  mChannelVisible = millis();
  // update the video source
  mVideoSource->setChannel(channel);
}

void VideoPlayer::play()
{
  if (mState == VideoPlayerState::PLAYING)
  {
    return;
  }
  mState = VideoPlayerState::PLAYING;
  mVideoSource->setState(VideoPlayerState::PLAYING);
}

void VideoPlayer::stop()
{
  if (mState == VideoPlayerState::STOPPED)
  {
    return;
  }
  mState = VideoPlayerState::STOPPED;
  mVideoSource->setState(VideoPlayerState::STOPPED);
  mDisplay->fillScreen(DisplayColors::BLACK);
}

void VideoPlayer::pause()
{
  if (mState == VideoPlayerState::PAUSED)
  {
    return;
  }
  mState = VideoPlayerState::PAUSED;
  mVideoSource->setState(VideoPlayerState::PAUSED);
}

void VideoPlayer::playStatic()
{
  if (mState == VideoPlayerState::STATIC)
  {
    return;
  }
  mState = VideoPlayerState::STATIC;
  mVideoSource->setState(VideoPlayerState::STATIC);
}


// double buffer the dma drawing otherwise we get corruption
uint16_t *dmaBuffer[2] = {NULL, NULL};
int dmaBufferIndex = 0;
int _doDraw(JPEGDRAW *pDraw)
{
  VideoPlayer *player = (VideoPlayer *)pDraw->pUser;
  player->mDisplay->drawJpg(pDraw->pPixels);
  return 1;
}

static unsigned short x = 12345, y = 6789, z = 42, w = 1729;

unsigned short xorshift16()
{
  unsigned short t = x ^ (x << 5);
  x = y;
  y = z;
  z = w;
  w = w ^ (w >> 1) ^ t ^ (t >> 3);
  return w & 0xFFFF;
}

void VideoPlayer::framePlayerTask()
{
  uint16_t *staticBuffer = NULL;
  uint8_t *jpegBuffer = NULL;
  size_t jpegBufferLength = 0;
  size_t jpegLength = 0;
  // used for calculating frame rate
  std::list<int> frameTimes;
  while (true)
  {
    if (mState == VideoPlayerState::STOPPED || mState == VideoPlayerState::PAUSED)
    {
      // nothing to do - just wait
      vTaskDelay(100 / portTICK_PERIOD_MS);
      continue;
    }
    if (mState == VideoPlayerState::STATIC)
    {
      // draw random pixels to the screen to simulate static
      // we'll do this 8 rows of pixels at a time to save RAM
      // TODO
      int width = 126;
      int height = 8;
      if (staticBuffer == NULL)
      {
        staticBuffer = (uint16_t *)malloc(width * height * 2);
      }
      for (int i = 0; i < 16; i++)
      {
        for (int p = 0; p < width * height; p++)
        {
          int grey = xorshift16() >> 8;
          staticBuffer[p] = mDisplay->color565(grey, grey, grey);
        }
      }
      // TODO
      mDisplay->drawJpg(staticBuffer);
      vTaskDelay(50 / portTICK_PERIOD_MS);
      continue;
    }
    // get the next frame
    if (!mVideoSource->getVideoFrame(&jpegBuffer, jpegBufferLength, jpegLength))
    {
      // no frame ready yet
      vTaskDelay(10 / portTICK_PERIOD_MS);
      continue;
    }
    frameTimes.push_back(millis());
    // keep the frame rate elapsed time to 5 seconds
    while(frameTimes.size() > 0 && frameTimes.back() - frameTimes.front() > 5000) {
      frameTimes.pop_front();
    }
    if (mJpeg.openRAM(jpegBuffer, jpegLength, _doDraw))
    {
      mJpeg.setUserPointer(this);
      mJpeg.setPixelType(RGB565_BIG_ENDIAN);
      mJpeg.decode(0, 0, 0);
      mJpeg.close();
    }
    // show channel indicator 
    if (millis() - mChannelVisible < 2000) {
      mDisplay->drawChannel(mChannelData->getChannelNumber());
    }
    #if CORE_DEBUG_LEVEL > 0
    mDisplay->drawFPS(frameTimes.size() / 5);
    #endif
  }
}
