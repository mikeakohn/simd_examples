/**
 *  Image processing routines.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: BSD
 *
 * Copyright 2018 by Michael Kohn
 *
 */

#include <stdint.h>

#include "pic_info.h"

void color_to_bw(uint8_t *image_bw, struct _pic_info *pic_info)
{
  int x;
  int length = pic_info->width * pic_info->height;

  for (x = 0; x < length; x++)
  {
    float r = pic_info->picture[x] & 0xff;
    float g = (pic_info->picture[x] >> 8) & 0xff;
    float b = (pic_info->picture[x] >> 16) & 0xff;

    float y = (0.299 * r) + (0.587 * g) + (0.114 * b);
    int y1 = (int)y;

    image_bw[x] = (y1 > 255) ? 255 : y1;
  }
}

