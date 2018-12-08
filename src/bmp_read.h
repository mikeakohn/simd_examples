/**
 *  Image processing routines.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: BSD
 *
 * Copyright 2007-2018 by Michael Kohn
 *
 */

#ifndef _BMP_READ_H
#define _BMP_READ_H

#include <stdint.h>

#include "pic_info.h"

struct _bitmap_file
{
  uint8_t type[2];
  uint32_t size;
  uint16_t reserved1;
  uint16_t reserved2;
  uint32_t offset;
};

struct _bitmap_info
{
  uint32_t size;
  uint32_t width;
  uint32_t height;
  uint16_t planes;
  uint16_t bit_count;
  uint32_t compression;
  uint32_t image_size;
  uint32_t x_pels_per_metre;
  uint32_t y_pels_per_metre;
  uint32_t colors_used;
  uint32_t colors_important;
  int colors[256];
};

int bmp_read(const char *filename, struct _pic_info *pic_info);

#endif

