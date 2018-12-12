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

global brightness_avx2

;void brightness_avx2(uint8_t *buffer, int length, int value)
brightness_avx2:
  test edx, edx
  jg brightness_sse_not_neg
  neg dl
brightness_sse_not_neg:

  ;; Create a 16 byte vector setting each byte to value.
  mov dh, dl
  mov eax, edx
  sal edx, 16
  or edx, eax

  vpinsrd xmm1, edx, 0
  vpinsrd xmm1, edx, 1
  vpinsrd xmm1, edx, 2
  vpinsrd xmm1, edx, 3
  vpinsrd xmm1, edx, 4
  vpinsrd xmm1, edx, 5
  vpinsrd xmm1, edx, 6
  vpinsrd xmm1, edx, 7

  test eax, eax
  jg brightness_sse_pos_loop

  ;; Iterate over image 16 pixels at a time and sub value from each pixel.
brightness_sse_neg_loop:
  vmovups ymm0, [rdi]
  vpsubusb ymm0, ymm1
  vmovups [rdi], ymm0
  add rdi, 32
  sub rsi, 32
  jnz brightness_sse_neg_loop
  ret

  ;; Iterate over image 16 pixels at a time and add value to each pixel.
brightness_sse_pos_loop:
  vmovups ymm0, [rdi]
  vpaddusb ymm0, ymm1
  vmovups [rdi], ymm0
  add rdi, 32
  sub rsi, 32
  jnz brightness_sse_pos_loop
  ret

