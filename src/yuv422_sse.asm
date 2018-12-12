;;
;;  Image processing routines.
;;  Author: Michael Kohn
;;   Email: mike@mikekohn.net
;;     Web: http://www.mikekohn.net/
;; License: BSD
;;
;; Copyright 2018 by Michael Kohn
;;

BITS 64

global yuv422_to_rgb24_float_sse
global yuv422_to_rgb24_int_sse

const_128: align 16
  dd 128.0, 128.0, 128.0, 120.0

const_v1: align 16
  dd 1.13983, 1.13983, 1.13983, 1.13983

const_uv1: align 16
  dd -0.39466, -0.39466, -0.39466, -0.39466

const_uv2: align 16
  dd 0.58060, 0.58060, 0.58060, 0.58060

const_u1: align 16
  dd 2.03211, 2.03211, 2.03211, 2.03211

;    v1 = (1.13983 * (float)v);
;    uv1 = -(0.39466 * (float)u) - (0.58060*(float)v);
;    u1 = (2.03211 * (float)u);

;void yuv422_to_rgb24_float_sse(uint8_t *image_rgb24, uint8_t *image_yuv422, int width, int height)
yuv422_to_rgb24_float_sse:
  ;; ecx = length of Y buffer
  mov eax, edx
  mul ecx
  sar ecx, 1
  ;; rdx points to U buffer
  mov rdx, rsi
  add rdx, rcx
  ;; r8 points to V buffer
  mov r8, rdx
  add r8, rcx

  pxor xmm0, xmm0        ; xmm0 = [ 0, 0, 0, 0 ]

yuv422_to_rgb24_float_sse_loop:

  movss xmm1, [rsi]
  movss xmm2, [rdi]
  movss xmm3, [r8]

  cvtdq2ps xmm1, xmm1    ; xmm1 = (float)vecy
  cvtdq2ps xmm2, xmm2    ; xmm2 = (float)vecu
  cvtdq2ps xmm3, xmm3    ; xmm3 = (float)vecv


  sub ecx, 4
  jnz yuv422_to_rgb24_float_sse_loop
  ret

;void yuv422_to_rgb24_int_sse(uint8_t *image_rgb24, uint8_t *image_yuv422, int width, int height)
yuv422_to_rgb24_int_sse:

  ret

