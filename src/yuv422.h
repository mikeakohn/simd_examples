/**
 *  Image processing routines.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: BSD
 *
 * Copyright 2007-2018 by Michael Kohn
 *
 * YUV422 is a planer format where the image_yuv422 is formatted as:
 *
 * Y bytes (length is width * height)
 * U bytes (length is width * height / 2)
 * V bytes (length is width * height / 2)
 *
 * Every U and V byte goes together with 2 Y bytes.  There is twice as
 * much Y (brightness) information than there is UV (color).
 *
 */

#ifndef _YUV422_H
#define _YUV422_H

#include <stdint.h>

int yuv422_read(const char *filename, uint8_t **image_yuv);

void yuv422_to_rgb32_float(
  uint32_t *image_rgb32,
  uint8_t *image_yuv422,
  int width,
  int height);

void yuv422_to_rgb32_int(
  uint32_t *image_rgb32,
  uint8_t *image_yuv422,
  int width,
  int height);

void yuv422_to_rgb32_float_sse(
  uint32_t *image_rgb32,
  uint8_t *image_yuv422,
  int width,
  int height);

void yuv422_to_rgb32_int_sse(
  uint32_t *image_rgb32,
  uint8_t *image_yuv422,
  int width,
  int height);

#endif

