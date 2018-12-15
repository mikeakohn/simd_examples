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

#include "timer.h"

double diff_time(struct timespec *tp_start, struct timespec *tp_stop)
{
  long nsec = tp_stop->tv_nsec - tp_start->tv_nsec;
  long sec = tp_stop->tv_sec - tp_start->tv_sec;

  if (nsec < 0) { sec--; nsec += 1000000000; }

  double t = (sec * 1000) + ((double)nsec / 1000000);

  return t;
}

