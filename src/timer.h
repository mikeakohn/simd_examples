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

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <time.h>

union _perftime
{
  struct _split
  {
    uint32_t lo;
    uint32_t hi;
  } split;
  uint64_t count;
};

#ifdef CYCLES_COUNT

#define TIMER_START \
  asm __volatile__ \
  ( \
    "rdtsc" : "=a" (perf_start.split.lo), "=d" (perf_start.split.hi) \
  ); \
  count = 0;

#define TIMER_STOP \
  asm __volatile__ \
  ( \
    "rdtsc" : "=a" (perf_end.split.lo), "=d" (perf_end.split.hi) \
  ); \
  printf("count=%d cpu=%ld\n", count, perf_end.count - perf_start.count); \
  fflush(stdout);

#else

#define TIMER_START \
  struct timespec tp_start; \
  struct timespec tp_stop; \
  clock_gettime(CLOCK_MONOTONIC, &tp_start); \

#define TIMER_STOP \
  clock_gettime(CLOCK_MONOTONIC, &tp_stop); \
  printf("msec=%f\n", diff_time(&tp_start, &tp_stop)); \
  fflush(stdout);

#endif

double diff_time(struct timespec *tp_start, struct timespec *tp_stop);

#endif

