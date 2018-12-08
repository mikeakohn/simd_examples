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

#include <stdint.h>

void brightness(uint8_t *buffer, int length, int value)
{
  int t, r;

  if (value > 0)
  {
    for (t = 0; t < length; t++)
    {
      r = buffer[t] + value;

      buffer[t] = (r > 255) ? 255 : r;
    }
  }
  else
  {
    for (t = 0; t < length; t++)
    {
      r = buffer[t] + value;

      buffer[t] = (r < 0) ? 0 : r;
    }
  }
}

