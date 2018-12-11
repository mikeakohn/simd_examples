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

#ifndef _BMP_WRITE_H
#define _BMP_WRITE_H

#include <stdint.h>

#include "pic_info.h"

int bmp_write(const char *filename, struct _pic_info *pic_info);
int bmp_write_bw(const char *filename, uint8_t *image_bw, int width, int height);
int bmp_write_rgb24(const char *filename, uint8_t *image_rgb24, int width, int height);

#endif

