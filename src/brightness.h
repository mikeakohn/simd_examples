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

#ifndef _BRIGHTNESS_H
#define _BRIGHTNESS_H

#include <stdint.h>

void brightness(uint8_t *buffer, int length, int value);
void brightness_sse(uint8_t *buffer, int length, int value);
void brightness_avx2(uint8_t *buffer, int length, int value);

#endif

