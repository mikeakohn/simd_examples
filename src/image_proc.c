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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "bmp_read.h"
#include "bmp_write.h"
#include "brightness.h"
#include "color_to_bw.h"
#include "pic_info.h"
#include "timer.h"
#include "yuv422.h"

//int test_sse(void *dest, void *src);

enum
{
  INSTRUCTIONS_UNKNOWN,
  INSTRUCTIONS_NORMAL,
  INSTRUCTIONS_SSE,
  INSTRUCTIONS_AVX,
  INSTRUCTIONS_AVX512,
};

enum
{
  FUNCTION_UNKNOWN,
  FUNCTION_BRIGHTNESS,
  FUNCTION_YUV,
};

enum
{
  TYPE_UNKNOWN,
  TYPE_FLOAT,
  TYPE_INT,
};

static int get_function(const char *function)
{
  if (strcmp(function, "brightness") == 0)
  {
    return FUNCTION_BRIGHTNESS;
  }
  else if (strcmp(function, "yuv") == 0)
  {
    return FUNCTION_YUV;
  }

  return FUNCTION_UNKNOWN;
}

static int get_instructions(const char *instructions)
{
  if (strcmp(instructions, "normal") == 0)
  {
    return INSTRUCTIONS_NORMAL;
  }
  else if (strcmp(instructions, "sse") == 0)
  {
    return INSTRUCTIONS_SSE;
  }
  else if (strcmp(instructions, "avx") == 0)
  {
    return INSTRUCTIONS_AVX;
  }

  return INSTRUCTIONS_UNKNOWN;
}

static int get_type(const char *type)
{
  if (strcmp(type, "int") == 0)
  {
    return TYPE_INT;
  }
  else if (strcmp(type, "float") == 0)
  {
    return TYPE_FLOAT;
  }

  return TYPE_UNKNOWN;
}

static void process_brightness(
  struct _pic_info *pic_info,
  int instructions,
  int value)
{
  uint8_t *image_bw;
  const int length = pic_info->width * pic_info->height;

  image_bw = (uint8_t *)malloc(length);
  color_to_bw(image_bw, pic_info);

TIMER_START

  if (instructions == INSTRUCTIONS_NORMAL)
  {
    brightness(image_bw, length, value);
  }
  else if (instructions == INSTRUCTIONS_SSE)
  {
    brightness_sse(image_bw, length, value);
  }
  else if (instructions == INSTRUCTIONS_AVX)
  {
    brightness_avx2(image_bw, length, value);
  }
  else
  {
    printf("Error: Unsupported instruction set.\n");
  }

TIMER_STOP

  bmp_write_bw("out.bmp", image_bw, pic_info->width, pic_info->height);

  free(image_bw);
}

static void process_yuv(uint8_t *image_yuv422, int instructions, int type, int width, int height)
{
  uint8_t *image_rgb24;

  image_rgb24 = (uint8_t *)malloc(width * height * 3);

TIMER_START

  if (instructions == INSTRUCTIONS_NORMAL)
  {
    if (type == TYPE_INT)
    {
      yuv422_to_rgb24_int(image_rgb24, image_yuv422, width, height);
    }
    else
    {
      yuv422_to_rgb24_float(image_rgb24, image_yuv422, width, height);
    }
  }
  else if (instructions == INSTRUCTIONS_SSE)
  {
    if (type == TYPE_INT)
    {
      yuv422_to_rgb24_int_sse(image_rgb24, image_yuv422, width, height);
    }
    else
    {
      yuv422_to_rgb24_float_sse(image_rgb24, image_yuv422, width, height);
    }
  }
  else
  {
    printf("Error: Unsupported instruction set.\n");
  }

TIMER_STOP

  bmp_write_rgb24("out.bmp", image_rgb24, width, height);

  free(image_rgb24);
}

#if 0
int test()
{
  int s[4];
  int d[4];

  s[0] = 1;
  s[1] = 2;
  s[2] = 3;
  s[3] = 4;

  d[0] = 5;
  d[1] = 6;
  d[2] = 7;
  d[3] = 8;

  test_sse(d, s);

#if 0
  float k = -0.39466;

  printf("%08x\n", *((uint32_t *)((void *)&k)));
#endif

  printf("test: %08x %08x %08x %08x\n",
    d[0],
    d[1],
    d[2],
    d[3]);

  float *f = (float *)d;

  printf("test: %f %f %f %f\n",
    f[0],
    f[1],
    f[2],
    f[3]);

  return 0;
}
#endif

int main(int argc, char *argv[])
{
  struct _pic_info pic_info;
  const char *filename;
  int function;
  int value;
  int instructions;
  int type;

  memset(&pic_info, 0, sizeof(pic_info));

  //test();

  if (argc != 6)
  {
    printf("Usage: %s <filename> <brightness/yuv> <value> <normal/sse/avx> <float/int>\n", argv[0]);
    printf(
      "   brightness: Convert .bmp image to bw and change brightness.\n"
      "          yuv: Convert yuv422 to rgb (value is image width).\n"
      "       normal: Use straight C.\n"
      "          sse: Use SSE/SSE2/SSE3/SSE4 instructions.\n"
      "          avx: Use AVX/AVX2 instructions.\n"
      "          int: Use all integer instructions.\n"
      "        float: Use float instructions for main parts of code.\n");
    exit(0);
  }

  filename = argv[1];
  function = get_function(argv[2]);
  value = atoi(argv[3]);
  instructions = get_instructions(argv[4]);
  type = get_type(argv[5]);

  if (function == FUNCTION_UNKNOWN)
  {
    printf("Error: function %s is unknown\n", argv[2]);
    exit(1);
  }

  if (instructions == INSTRUCTIONS_UNKNOWN)
  {
    printf("Error: instruction set %s is unknown\n", argv[4]);
    exit(1);
  }

  if (type == TYPE_UNKNOWN)
  {
    printf("Error: Type %s is unknown\n", argv[5]);
    exit(1);
  }

  if (function == FUNCTION_BRIGHTNESS)
  {
    if (type == TYPE_FLOAT)
    {
      printf("Float isn't valid for brightness.\n");
      exit(1);
    }

    bmp_read(filename, &pic_info);

    process_brightness(&pic_info, instructions, value);

    pic_info_free(&pic_info);
  }
  else if (function == FUNCTION_YUV)
  {
    uint8_t *image_yuv422;
    int length;
    int width, height;

    length = yuv422_read(filename, &image_yuv422);

    if (length != 0)
    {
      width = value;
      height = (length / 2) / width;

      process_yuv(image_yuv422, instructions, type, width, height);
      free(image_yuv422);
    }
  }

  return 0;
}

