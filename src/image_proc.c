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
#include "color_to_bw.h"
#include "pic_info.h"

enum
{
  INSTRUCTION_UNKNOWN,
  INSTRUCTION_NORMAL,
  INSTRUCTION_SSE,
  INSTRUCTION_AVX,
  INSTRUCTION_AVX2,
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
    return INSTRUCTION_NORMAL;
  }
  else if (strcmp(instructions, "sse") == 0)
  {
    return INSTRUCTION_SSE;
  }
  else if (strcmp(instructions, "avx") == 0)
  {
    return INSTRUCTION_AVX;
  }
  else if (strcmp(instructions, "avx2") == 0)
  {
    return INSTRUCTION_AVX2;
  }

  return INSTRUCTION_UNKNOWN;
}

int main(int argc, char *argv[])
{
  struct _pic_info pic_info;
  uint8_t *image_bw;
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

  bmp_read(filename, &pic_info);

  image_bw = (uint8_t *)malloc(pic_info.width * pic_info.height);

  color_to_bw(image_bw, &pic_info);

  bmp_write_bw("out.bmp", image_bw, pic_info.width, pic_info.height);

  pic_info_free(&pic_info);

  return 0;
}

