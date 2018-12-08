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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "bmp_write.h"

struct _bmp
{
  FILE *out;
  int curr_x;
  int total_x;
};

static int write_chars(FILE *out, char *s)
{
  int t;

  t = 0;

  while(s[t] != 0 && t < 255)
  {
    putc(s[t++], out);
  }

  return 0;
}

static int write_int32(FILE *out, int n)
{
  putc(n & 0xff, out);
  putc((n >> 8) & 0xff, out);
  putc((n >> 16) & 0xff, out);
  putc((n >> 24) & 0xff, out);

  return 0;
}

static int write_int16(FILE *out, int n)
{
  putc(n & 255, out);
  putc((n >> 8) & 0xff, out);

  return 0;
}

static void write_bmp_header(struct _bmp *bmp, int width, int height)
{
  int image_width;

  image_width = width;
  while ((image_width % 4) != 0) { image_width++; }
  bmp->total_x = width * 3;

  // size: 14 bytes 

  write_chars(bmp->out, "BM");
  write_int32(bmp->out, 0);
  write_int16(bmp->out, 0);
  write_int16(bmp->out, 0);
  write_int32(bmp->out, 54);

  // head1: 14  head2: 40

  write_int32(bmp->out, 40);
  write_int32(bmp->out, width);
  write_int32(bmp->out, height);
  write_int16(bmp->out, 1);
  write_int16(bmp->out, 24);
  write_int32(bmp->out, 0);          // compression
  write_int32(bmp->out, image_width*height);
  write_int32(bmp->out, 0);          // biXPelsperMetre
  write_int32(bmp->out, 0);          // biYPelsperMetre
  write_int32(bmp->out, 0);
  write_int32(bmp->out, 0);

/*
  for (t = 0; t < 256; t++)
  {
    write_int32(bmp->out, palette[t]);
  }
*/

  bmp->curr_x = 0;
}

static void write_bmp_pixel(struct _bmp *bmp, uint8_t red, uint8_t green, uint8_t blue)
{
  putc(blue, bmp->out);
  putc(green, bmp->out);
  putc(red, bmp->out);

  bmp->curr_x += 3;

  if (bmp->curr_x == bmp->total_x)
  {
    while ((bmp->curr_x % 4) != 0)
    {
      putc(0, bmp->out);
      bmp->curr_x++;
    }

    bmp->curr_x = 0;
  }
}

static int write_bmp_footer(struct _bmp *bmp)
{
  long marker;

  marker = ftell(bmp->out);
  fseek(bmp->out, 2, SEEK_SET);
  write_int32(bmp->out, marker);
  fseek(bmp->out, marker, SEEK_SET);

  return 0;
}

int bmp_write(const char *filename, struct _pic_info *pic_info)
{
  struct _bmp bmp;
  int x, y;
  const int width = pic_info->width;
  const int height = pic_info->height;
  const uint32_t *picture = pic_info->picture;

  bmp.out = fopen(filename, "wb");

  if (bmp.out == NULL)
  {
    printf("Cannot open file %s for writing.\n", filename);
    return -1;
  }

  write_bmp_header(&bmp, width, height);

  for (y = height - 1; y >= 0; y--)
  {
    int ptr = y * width;

    for (x = 0; x < width; x++)
    {
      write_bmp_pixel(&bmp,
                      picture[ptr] & 0xff,
                     (picture[ptr] >> 8) & 0xff,
                     (picture[ptr] >> 16) & 0xff);
      ptr++;
    }
  }

  write_bmp_footer(&bmp);

  fclose(bmp.out);

  return 0;
}

int bmp_write_bw(const char *filename, uint8_t *image_bw, int width, int height)
{
  struct _bmp bmp;
  int x, y;

  bmp.out = fopen(filename, "wb");

  if (bmp.out == NULL)
  {
    printf("Cannot open file %s for writing.\n", filename);
    return -1;
  }

  write_bmp_header(&bmp, width, height);

  for (y = height - 1; y >= 0; y--)
  {
    int ptr = y * width;

    for (x = 0; x < width; x++)
    {
      write_bmp_pixel(&bmp, image_bw[ptr], image_bw[ptr], image_bw[ptr]);
      ptr++;
    }
  }

  write_bmp_footer(&bmp);

  fclose(bmp.out);

  return 0;
}

