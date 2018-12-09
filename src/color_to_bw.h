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

#ifndef _COLOR_TO_BW_H
#define _COLOR_TO_BW_H

#include <stdint.h>

#include "pic_info.h"

void color_to_bw(uint8_t *image_bw, struct _pic_info *pic_info);

#endif

