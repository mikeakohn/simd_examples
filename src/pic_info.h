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

#ifndef _PIC_INFO_H
#define _PIC_INFO_H

#include <stdint.h>

struct _pic_info
{ 
  int width;
  int height;
  uint32_t *picture;
};

#endif

