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

global _brightness_sse
global brightness_sse

;void brightness_sse(uint8_t *buffer, int length, int value)
_brightness_sse:
brightness_sse:
  test edx, edx
  jg brightness_sse_not_neg
  neg dl
brightness_sse_not_neg:

  ;; Create a 16 byte vector setting each byte to value.  Inserts a
  ;; dword of vvvv into vector element 0 and then uses shuffle dword
  ;; to copy that value to the other vector dword elements.
  mov dh, dl
  pinsrw xmm1, dx, 0
  pinsrw xmm1, dx, 1
  pshufd xmm1, xmm1, 0

  test edx, edx
  jg brightness_sse_pos_loop

  ;; Iterate over image 16 pixels at a time and sub value from each pixel.
brightness_sse_neg_loop:
  movups xmm0, [rdi]
  psubusb xmm0, xmm1
  movups [rdi], xmm0
  add rdi, 16
  sub rsi, 16
  jnz brightness_sse_neg_loop
  ret

  ;; Iterate over image 16 pixels at a time and add value to each pixel.
brightness_sse_pos_loop:
  movups xmm0, [rdi]
  paddusb xmm0, xmm1
  movups [rdi], xmm0
  add rdi, 16
  sub rsi, 16
  jnz brightness_sse_pos_loop
  ret

