#include "nGlass.h"
/**
 * @file      GlassWindown.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  ShenZhen XinYuan Electronic Technology Co., Ltd
 * @date      2024-02-22
 *
 */

NGlass nGlass;

void setup()
{
  nGlass.init();
}

void loop()
{
  nGlass.tick();
}
