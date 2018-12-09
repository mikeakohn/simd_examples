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

enum
{
  INSTRUCTIONS_UNKNOWN,
  INSTRUCTIONS_NORMAL,
  INSTRUCTIONS_SSE,
  INSTRUCTIONS_AVX,
  INSTRUCTIONS_AVX2,
};

enum
{
  FUNCTION_UNKNOWN,
  FUNCTION_BRIGHTNESS,
  FUNCTION_YUV,
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
  else if (strcmp(instructions, "avx2") == 0)
  {
    return INSTRUCTIONS_AVX2;
  }

  return INSTRUCTIONS_UNKNOWN;
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

  if (instructions == INSTRUCTIONS_NORMAL)
  {
    brightness(image_bw, length, value);
  }
  else if (instructions == INSTRUCTIONS_SSE)
  {
    brightness_sse(image_bw, length, value);
  }
  else
  {
    printf("Error: Unsupported instruction set.\n");
  }

  bmp_write_bw("out.bmp", image_bw, pic_info->width, pic_info->height);

  free(image_bw);
}

static void process_yuv(struct _pic_info *pic_info, int instructions)
{
}

int main(int argc, char *argv[])
{
  struct _pic_info pic_info;
  const char *filename;
  int function;
  int value;
  int instructions;

  memset(&pic_info, 0, sizeof(pic_info));

  if (argc != 5)
  {
    printf("Usage: %s <filename> <brightness/yuv> <value> <normal/sse/avx/avx2>\n", argv[0]);
    printf("   brightness: Convert image to bw and change brightness\n");
    printf("          yuv: Convert yuv422 to rgb (value is ignored)\n");
    printf("       normal: Use straight C.\n");
    printf("          sse: Use SSE instructions\n");
    printf("          avx: Use AVX instructions\n");
    printf("         avx2: Use AVX2 instructions\n");
    exit(0);
  }

  filename = argv[1];
  function = get_function(argv[2]);
  value = atoi(argv[3]);
  instructions = get_instructions(argv[4]);

  if (function == FUNCTION_UNKNOWN)
  {
    printf("Error: %s is unknown\n", argv[2]);
    exit(1);
  }

  if (instructions == INSTRUCTIONS_UNKNOWN)
  {
    printf("Error: %s is unknown\n", argv[4]);
    exit(1);
  }

  bmp_read(filename, &pic_info);

  if (function == FUNCTION_BRIGHTNESS)
  {
    process_brightness(&pic_info, instructions, value);
  }
  else if (function == FUNCTION_YUV)
  {
    process_yuv(&pic_info, instructions);
  }

  pic_info_free(&pic_info);

  return 0;
}

