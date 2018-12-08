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

#include "bmp.h"

static int read_int32(FILE *in)
{
  int c;

  c = getc(in);
  c |= getc(in) << 8;
  c |= getc(in) << 16;
  c |= getc(in) << 24;

  return c;
}

static int read_int16(FILE *in)
{
  int c;

  c = getc(in);
  c |= getc(in) << 8;

  return c;
}

static void bmp_read_file_header(FILE *in, struct _bitmap_file *bitmap_file)
{
  bitmap_file->type[0] = getc(in);
  bitmap_file->type[1] = getc(in);
  bitmap_file->size = read_int32(in);
  bitmap_file->reserved1 = read_int16(in);
  bitmap_file->reserved2 = read_int16(in);
  bitmap_file->offset = read_int32(in);
}

static void bmp_read_info_header(FILE *in, struct _bitmap_info *bitmap_info)
{
  bitmap_info->size = read_int32(in);
  bitmap_info->width = read_int32(in);
  bitmap_info->height = read_int32(in);
  bitmap_info->planes = read_int16(in);
  bitmap_info->bit_count = read_int16(in);
  bitmap_info->compression = read_int32(in);
  bitmap_info->image_size = read_int32(in);
  bitmap_info->x_pels_per_metre = read_int32(in);
  bitmap_info->y_pels_per_metre = read_int32(in);
  bitmap_info->colors_used = read_int32(in);
  bitmap_info->colors_important = read_int32(in);
}

static void bmp_raw_uncompressed(FILE *in, uint32_t *picture, struct _bitmap_info *bitmap_info)
{
  int x, y;
  int c = 0,t;
  int byte_count;

  for (y = bitmap_info->height - 1; y >= 0; y--)
  {
    byte_count = 0;

    for (x = 0; x < bitmap_info->width; x++)
    {
      if (bitmap_info->bit_count == 8 ||
          bitmap_info->bit_count == 24 ||
          bitmap_info->bit_count == 32)
      {
        if (bitmap_info->bit_count==8) 
        {
          c = getc(in);
          c = bitmap_info->colors[c];
          byte_count++;
        }
          else
        if (bitmap_info->bit_count == 24)
        {
          c = getc(in) + (getc(in) << 8) + (getc(in) << 16);
          byte_count = byte_count + 3;
        }
        else if (bitmap_info->bit_count == 32)
        {
          c = getc(in) + (getc(in) << 8) + (getc(in) << 16);
          t = getc(in);

          byte_count = byte_count + 4;
        }

        picture[x + (y * bitmap_info->width)] = c;
      }
      else if (bitmap_info->bit_count == 4)
      {
        c = getc(in);
        byte_count++;

        picture[x + (y * bitmap_info->width)] = bitmap_info->colors[(( c >> 4) & 15)];
        x++;

        if (x < bitmap_info->width)
        {
          picture[x + (y * bitmap_info->width)] = bitmap_info->colors[c & 15];
        }
      }
      else if (bitmap_info->bit_count == 1)
      {
        c = getc(in);
        byte_count++;

        for (t = 7; t >= 0; t--)
        {
          if (x < bitmap_info->width)
          {
            const int index = x + (y * bitmap_info->width);
            if (((c >> t) & 1) == 0)
            {
              picture[index] = bitmap_info->colors[0];
            }
            else
            {
              picture[index] = bitmap_info->colors[1];
            }
          }

          x++;
        }

        x--;
      }
    }

    c = (byte_count % 4);

    if (c != 0)
    {
      for (t = c; t < 4; t++)
      {
        getc(in);
      }
    }
  }
}

static void bmp_raw_compressed(FILE *in, uint32_t *picture, struct _bitmap_info *bitmap_info)
{
  int x, y;
  int c, t, r;

  y = bitmap_info->height - 1;
  x = 0;

  while (1)
  {
    c = getc(in);

    if (c == EOF) { return; }

    if (c != 0)
    {
      r = getc(in);

      for (t = 0; t < c; t++)
      {
        const int index = x + (y * bitmap_info->width);

        if (bitmap_info->bit_count == 4)
        {
          if ((t & 1) == 0)
          {
            picture[index] = bitmap_info->colors[r >> 4];
          }
          else
          {
            picture[index] = bitmap_info->colors[r & 15];
          }
        }
          else
        if (bitmap_info->bit_count == 8)
        {
          picture[index] = bitmap_info->colors[r];
        }

        x++;
      }
    }
      else
    {
      r = getc(in);

      if (r == 0)
      {
        x = 0;
        y--;
        continue;
      }
      else if (r == 1)
      {
        break;
      }
      else if (r == 2)
      {
        x = x + getc(in);
        y = y - getc(in);

        return;
      }

      for (t = 0; t < r; t++)
      {
        c = getc(in);

        const int index = x + (y * bitmap_info->width);

        if (bitmap_info->bit_count == 8)
        {
          picture[index] = bitmap_info->colors[c];
        }
        else if (bitmap_info->bit_count == 4)
        {
          picture[index] = bitmap_info->colors[c >> 4];

          t++;

          if (t<r)
          {
            x++;
            picture[index] = bitmap_info->colors[c & 15];
          }
        }

        x++;
      }

      if (bitmap_info->bit_count == 8)
      {
        c = r % 2;
      }
      else if (bitmap_info->bit_count == 4)
      {
        t = (r / 2) + (r % 2);
        c = t % 2;
      }

      if (c != 0)
      {
        getc(in);
      }
    }
  }
}

int bmp_load(FILE *in, struct _pic_info *pic_info)
{
  struct _bitmap_file bitmap_file;
  struct _bitmap_info bitmap_info;
  int t;

  bmp_read_file_header(in, &bitmap_file);

  if (bitmap_file.type[0] != 'B' || bitmap_file.type[1] != 'M')
  {
    printf("Not a bitmap.\n");
    return -1;
  }

  bmp_read_info_header(in, &bitmap_info);

#ifdef DEBUG
  printf("Bitmap File Header\n");
  printf("----------------------------------------------\n");
  printf("         bfType: %c%c\n", bitmap_file.type[0],
                                    bitmap_file.type[1]);
  printf("         bfSize: %d\n", bitmap_file.size);
  printf("      reserved1: %d\n", bitmap_file.reserved1);
  printf("      reserved2: %d\n", bitmap_file.reserved2);
  printf("         bfOffs: %d\n", bitmap_file.offset);
  printf("----------------------------------------------\n");
  printf("Bitmap Info Header\n");
  printf("----------------------------------------------\n");
  printf("         biSize: %d\n", bitmap_info.size);
  printf("        biWidth: %d\n", bitmap_info.width);
  printf("       biHeight: %d\n", bitmap_info.height);
  printf("       biPlanes: %d\n", bitmap_info.planes);
  printf("     biBitCount: %d\n", bitmap_info.bit_count);
  printf("  biCompression: %d\n", bitmap_info.compression);
  printf("    biSizeImage: %d\n", bitmap_info.image_size);
  printf("biXPelsPerMetre: %d\n", bitmap_info.x_pels_per_metre);
  printf("biYPelsPerMetre: %d\n", bitmap_info.y_pels_per_metre);
  printf("      biClrUsed: %d\n", bitmap_info.colors_used);
  printf(" biClrImportant: %d\n", bitmap_info.colors_important);
  printf("----------------------------------------------\n");
#endif

  bitmap_info.colors[0] = 0;
  bitmap_info.colors[1] = 0xffffff;
  bitmap_info.colors[255] = 0xffffff;

  if (bitmap_info.colors_important == 0 && bitmap_info.bit_count == 8)
  {
    bitmap_info.colors_important = 256;
  }
  
  for (t = 0; t < bitmap_info.colors_important; t++)
  {
    bitmap_info.colors[t] = read_int32(in);
  }

  pic_info->width = bitmap_info.width;
  pic_info->height = bitmap_info.height;

  pic_info->picture = malloc(bitmap_info.width * bitmap_info.height * sizeof(int));
  // posix_memalign((void **)&pic_info->picture, 16, bitmap_info.width*bitmap_info.height * sizeof(int));

  fseek(in, bitmap_file.offset, 0);

  if (bitmap_info.compression == 0)
  {
    bmp_raw_uncompressed(in, pic_info->picture, &bitmap_info);
  }
  else if (bitmap_info.compression == 1)
  {
    bmp_raw_compressed(in, pic_info->picture, &bitmap_info);
  }
  else if (bitmap_info.compression == 2)
  {
    bmp_raw_compressed(in, pic_info->picture, &bitmap_info);
  }
  else if (bitmap_info.compression == 3)
  {
    bmp_raw_uncompressed(in, pic_info->picture, &bitmap_info);
  }
  else
  {
    printf("This type of compression is not supported at this time.\n");
    return 0;
  }

  return 0;
}

