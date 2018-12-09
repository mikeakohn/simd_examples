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

global brightness_sse

;void brightness(uint8_t *buffer, int length, int value)
brightness_sse:
  test edx, edx
  jg brightness_sse_not_neg
  neg dl
brightness_sse_not_neg:

  mov dh, dl
  pinsrw xmm1, dx, 0
  pinsrw xmm1, dx, 1
  pinsrw xmm1, dx, 2
  pinsrw xmm1, dx, 3
  pinsrw xmm1, dx, 4
  pinsrw xmm1, dx, 5
  pinsrw xmm1, dx, 6
  pinsrw xmm1, dx, 7

  test edx, edx
  jg brightness_sse_pos_loop

brightness_sse_neg_loop:
  movups xmm0, [rdi]
  psubusb xmm0, xmm1
  movups [rdi], xmm0
  add rdi, 16
  sub rsi, 16
  jnz brightness_sse_neg_loop
  ret

brightness_sse_pos_loop:
  movups xmm0, [rdi]
  paddusb xmm0, xmm1
  movups [rdi], xmm0
  add rdi, 16
  sub rsi, 16
  jnz brightness_sse_pos_loop
  ret

