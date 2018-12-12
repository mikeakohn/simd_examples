/**
 *  Image processing routines.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: BSD
 *
 * Copyright 2007-2018 by Michael Kohn
 *
 * YUV422 is a planer format where the yuv_buffer is formatted as:
 *
 * Y bytes (length is width * height)
 * U bytes (length is width * height / 2)
 * V bytes (length is width * height / 2)
 *
 * Every U and V byte goes together with 2 Y bytes.  There is twice as
 * much Y (brightness) information than there is UV (color).
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int yuv422_read(const char *filename, uint8_t **image_yuv422)
{
  FILE *in;
  long length;

  *image_yuv422 = NULL;

  in = fopen(filename, "rb");

  if (in == NULL)
  {
    printf("Could not open %s for reading.\n", filename);
    return 0;
  }

  fseek(in, 0, SEEK_END);
  length = ftell(in);
  fseek(in, 0, SEEK_SET);

  *image_yuv422 = (uint8_t *)malloc(length);

  length = fread(*image_yuv422, 1, length, in);
  fclose(in);

  return length;
}

void yuv422_to_rgb24_float(
  uint8_t *rgb_buffer,
  uint8_t *yuv_buffer,
  int width,
  int height)
{
  uint8_t *u_buffer, *v_buffer;
  float u1, uv1, v1;
  float u, v;
  int length;
  int r, g, b;
  int rgb_ptr, y_ptr, n;

  length = width * height;

  u_buffer = yuv_buffer + length;
  length = length / 2;
  v_buffer = u_buffer + length;

  rgb_ptr = 0;
  y_ptr = 0;

  for (n = 0; n < length; n++)
  {
    // Process 2 pixels at a time.
    // Compute parts of the UV components.

    u = u_buffer[n] - 128;
    v = v_buffer[n] - 128;

    v1 = (1.13983 * (float)v);
    uv1 = -(0.39466 * (float)u) - (0.58060*(float)v);
    u1 = (2.03211 * (float)u);

    // Run even pixel through formula.

    r = yuv_buffer[y_ptr] + v1;
    g = yuv_buffer[y_ptr] + uv1;
    b = yuv_buffer[y_ptr] + u1;

    // Saturate pixels to a value between 0 and 255.
    r = (r > 255) ? 255 : r;
    g = (g > 255) ? 255 : g;
    b = (b > 255) ? 255 : b;

    r = (r < 0) ? 0 : r;
    g = (g < 0) ? 0 : g;
    b = (b < 0) ? 0 : b;

    rgb_buffer[rgb_ptr + 0] = r;
    rgb_buffer[rgb_ptr + 1] = g;
    rgb_buffer[rgb_ptr + 2] = b;

    // Run odd pixel through formula.

    r = yuv_buffer[y_ptr + 1] + v1;
    g = yuv_buffer[y_ptr + 1] + uv1;
    b = yuv_buffer[y_ptr + 1] + u1;

    // Saturate pixels to a value between 0 and 255.
    r = (r > 255) ? 255 : r;
    g = (g > 255) ? 255 : g;
    b = (b > 255) ? 255 : b;

    r = (r < 0) ? 0 : r;
    g = (g < 0) ? 0 : g;
    b = (b < 0) ? 0 : b;

    rgb_buffer[rgb_ptr + 3] = r;
    rgb_buffer[rgb_ptr + 4] = g;
    rgb_buffer[rgb_ptr + 5] = b;

    rgb_ptr += 6;
    y_ptr += 2;
  }
}

void yuv422_to_rgb24_int(
  uint8_t *rgb_buffer,
  uint8_t *yuv_buffer,
  int width,
  int height)
{
  uint8_t *u_buffer, *v_buffer;
  int u1, uv1, v1;
  int u, v;
  int length;
  int r, g, b;
  int rgb_ptr, y_ptr, n;
  int y1;

  length = width * height;

  u_buffer = yuv_buffer + length;
  length = length / 2;
  v_buffer =u_buffer + length;

  rgb_ptr = 0;
  y_ptr = 0;

  for (n = 0; n < length; n++)
  {
    // Process 2 pixels at a time.
    // Compute parts of the UV components.

    u = u_buffer[n] - 128;
    v = v_buffer[n] - 128;

    v1 = (5727 * v);
    uv1= - (1617 * u) - (2378 * v);
    u1 = (8324 * u);

    // Run even pixel through formula.

    y1 = yuv_buffer[y_ptr] << 12;

    r = (y1 + v1) >> 12;
    g = (y1 + uv1) >> 12;
    b = (y1 + u1) >> 12;

    // Saturate pixels to a value between 0 and 255.
    r = (r > 255) ? 255 : r;
    g = (g > 255) ? 255 : g;
    b = (b > 255) ? 255 : b;

    r = (r < 0) ? 0 : r;
    g = (g < 0) ? 0 : g;
    b = (b < 0) ? 0 : b;

    rgb_buffer[rgb_ptr + 0] = r;
    rgb_buffer[rgb_ptr + 1] = g;
    rgb_buffer[rgb_ptr + 2] = b;

    // Run odd pixel through formula.

    y1 = yuv_buffer[y_ptr + 1] << 12;

    r = (y1 + v1) >> 12;
    g = (y1 + uv1) >> 12;
    b = (y1 + u1) >> 12;

    // Saturate pixels to a value between 0 and 255.
    r = (r > 255) ? 255 : r;
    g = (g > 255) ? 255 : g;
    b = (b > 255) ? 255 : b;

    r = (r < 0) ? 0 : r;
    g = (g < 0) ? 0 : g;
    b = (b < 0) ? 0 : b;

    rgb_buffer[rgb_ptr + 3] = r;
    rgb_buffer[rgb_ptr + 4] = g;
    rgb_buffer[rgb_ptr + 5] = b;

    rgb_ptr += 6;
    y_ptr += 2;
  }
}

