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

global _yuv422_to_rgb24_float_sse
global yuv422_to_rgb24_float_sse
global _yuv422_to_rgb24_int_sse
global yuv422_to_rgb24_int_sse
global test_sse

;default rel

;const_shuffle_y: align 16
;  db 0x00, 0x01,  0x02, 0x03,  0x00, 0x01,  0x02, 0x03

;const_128: align 16
;  dd 128, 128, 128, 120

;const_v1: align 16
;  dd 1.13983, 1.13983, 1.13983, 1.13983

;const_uv1: align 16
;  dd -0.39466, -0.39466, -0.39466, -0.39466

;const_uv2: align 16
;  dd 0.58060, 0.58060, 0.58060, 0.58060

;const_u1: align 16
;  dd 2.03211, 2.03211, 2.03211, 2.03211

;    v1 = (1.13983 * (float)v);
;    uv1 = -(0.39466 * (float)u) - (0.58060*(float)v);
;    u1 = (2.03211 * (float)u);

;void yuv422_to_rgb24_float_sse(uint8_t *image_rgb24, uint8_t *image_yuv422, int width, int height)
_yuv422_to_rgb24_float_sse:
yuv422_to_rgb24_float_sse:
  ;; rdi = image_rgb24
  ;; rsi = image_yuv422
  ;; rdx = width
  ;; rcx = height

  ;; ecx = length of Y buffer
  imul ecx, edx
  ;; rdx points to U buffer (y + length)
  mov rdx, rsi
  add rdx, rcx
  ;; r8 points to V buffer (u + (length / 2))
  mov eax, ecx
  sar eax, 1
  mov r8, rdx
  add r8, rax

  ;; Load constants
  pxor xmm0, xmm0        ; xmm0 = [ 0, 0, 0, 0 ]

  mov eax, 128
  movd xmm1, eax
  pshufd xmm1, xmm1, 0   ; xmm1 = [ 128, 128, 128, 128 ]

  mov eax, 255
  movd xmm15, eax
  pshufd xmm15, xmm15, 0 ; xmm15 = [ 255, 255, 255, 255 ]

  mov eax, 0x3f91e5f3    ; 1.13983
  movd xmm8, eax
  pshufd xmm8, xmm8, 0   ; xmm8 = [ 1.13983, 1.13983, 1.13983, 1.13983 ]

  mov eax, 0xbeca10e0    ; -0.39466
  movd xmm9, eax
  pshufd xmm9, xmm9, 0   ; xmm9 = [ -0.39466, -0.39466, -0.39466, -0.39466 ]

  mov eax, 0xbf14a234    ; -0.58060
  movd xmm10, eax
  pshufd xmm10, xmm10, 0 ; xmm10 = [ -0.58060, -0.58060, -0.58060, -0.58060 ]

  mov eax, 0x40020e17    ; 2.03211
  movd xmm11, eax
  pshufd xmm11, xmm11, 0 ; xmm11 = [ 2.03211, 2.03211, 2.03211, 2.03211 ]

yuv422_to_rgb24_float_sse_loop:

  ;; Load YYYY data from memory.
  movss xmm2, [rsi]

  ;; Unpack YYYY into [ Y, Y, Y, Y ].
  punpcklbw xmm2, xmm0
  punpcklwd xmm2, xmm0

  ;; Load U0U1 into xmm3 [ U0, U0, U1, U1 ].
  mov ax, [rdx]
  pinsrw xmm3, ax, 0
  punpcklbw xmm3, xmm0
  punpcklwd xmm3, xmm0
  pshufd xmm3, xmm3, 0x50

  ;; Load V0V1 into xmm4 [ V0, V0, V1, V1 ].
  mov ax, [r8]
  pinsrw xmm4, ax, 0
  punpcklbw xmm4, xmm0
  punpcklwd xmm4, xmm0
  pshufd xmm4, xmm4, 0x50

  ;; [ U, U, U, U ] - [ 128, 128, 128, 128 ]
  ;; [ V, V, V, V ] - [ 128, 128, 128, 128 ]
  psubd xmm3, xmm1
  psubd xmm4, xmm1

  ;; Convert to float.
  cvtdq2ps xmm2, xmm2    ; xmm2 = (float)vecy
  cvtdq2ps xmm3, xmm3    ; xmm3 = (float)vecu
  cvtdq2ps xmm4, xmm4    ; xmm4 = (float)vecv

  ; xmm5 = uv1 = -(0.39466 * (float)u) - (0.58060*(float)v);
  movaps xmm5, xmm3
  mulps xmm5, xmm9
  movaps xmm6, xmm4
  mulps xmm6, xmm10
  addps xmm5, xmm6

  ; xmm3 = u1 = (2.03211 * (float)u);
  mulps xmm3, xmm11

  ; xmm4 = v1 = (1.13983 * (float)v);
  mulps xmm4, xmm8

  ; r = yuv_buffer[y_ptr] + v1; (xmm4)
  addps xmm4, xmm2

  ; g = yuv_buffer[y_ptr] + uv1; (xmm5)
  addps xmm5, xmm2

  ; b = yuv_buffer[y_ptr] + u1; (xmm3)
  addps xmm3, xmm2

  cvttps2dq xmm4, xmm4     ; float to int r
  cvttps2dq xmm5, xmm5     ; float to int b
  cvttps2dq xmm3, xmm3     ; float to int g

  pmaxsd xmm4, xmm0        ; if (r < 0) { r = 0; }
  pmaxsd xmm5, xmm0        ; if (g < 0) { g = 0; }
  pmaxsd xmm3, xmm0        ; if (b < 0) { b = 0; }

  pminsd xmm4, xmm15       ; if (r > 255) { r = 255; }
  pminsd xmm5, xmm15       ; if (g > 255) { g = 255; }
  pminsd xmm3, xmm15       ; if (b > 255) { b = 255; }

  pextrb eax, xmm4, 0
  mov [rdi+0], al
  pextrb eax, xmm5, 0
  mov [rdi+1], al
  pextrb eax, xmm3, 0
  mov [rdi+2], al

  pextrb eax, xmm4, 4
  mov [rdi+3], al
  pextrb eax, xmm5, 4
  mov [rdi+4], al
  pextrb eax, xmm3, 4
  mov [rdi+5], al

  pextrb eax, xmm4, 8
  mov [rdi+6], al
  pextrb eax, xmm5, 8
  mov [rdi+7], al
  pextrb eax, xmm3, 8
  mov [rdi+8], al

  pextrb eax, xmm4, 12
  mov [rdi+9], al
  pextrb eax, xmm5, 12
  mov [rdi+10], al
  pextrb eax, xmm3, 12
  mov [rdi+11], al

  add rdi, 12
  add rsi, 4
  add rdx, 2
  add r8, 2
  sub ecx, 4
  jnz yuv422_to_rgb24_float_sse_loop
  ret

;void yuv422_to_rgb24_int_sse(uint8_t *image_rgb24, uint8_t *image_yuv422, int width, int height)
_yuv422_to_rgb24_int_sse:
yuv422_to_rgb24_int_sse:
  ;; rdi = image_rgb24
  ;; rsi = image_yuv422
  ;; rdx = width
  ;; rcx = height

  ;; ecx = length of Y buffer
  imul ecx, edx
  ;; rdx points to U buffer (y + length)
  mov rdx, rsi
  add rdx, rcx
  ;; r8 points to V buffer (u + (length / 2))
  mov eax, ecx
  sar eax, 1
  mov r8, rdx
  add r8, rax

  ;; Load constants
  pxor xmm0, xmm0        ; xmm0 = [ 0, 0, 0, 0 ]

  mov eax, 128
  movd xmm1, eax
  pshufd xmm1, xmm1, 0   ; xmm1 = [ 128, 128, 128, 128 ]

  mov eax, 255
  movd xmm15, eax
  pshufd xmm15, xmm15, 0 ; xmm15 = [ 255, 255, 255, 255 ]

  mov eax, 5727
  movd xmm8, eax
  pshufd xmm8, xmm8, 0   ; xmm8 = [ 5727, 5727, 5727, 5727 ]

  mov eax, 1617
  movd xmm9, eax
  pshufd xmm9, xmm9, 0   ; xmm9 = [ 1617, 1617, 1617, 1617 ]

  mov eax, 2378
  movd xmm10, eax
  pshufd xmm10, xmm10, 0 ; xmm10 = [ 2378, 2378, 2378, 2378 ]

  mov eax, 8324
  movd xmm11, eax
  pshufd xmm11, xmm11, 0 ; xmm11 = [ 8324, 8324, 8324, 8324 ]

yuv422_to_rgb24_int_sse_loop:

  ;; Load YYYY data from memory.
  movss xmm2, [rsi]

  ;; Unpack YYYY into [ Y, Y, Y, Y ].
  punpcklbw xmm2, xmm0
  punpcklwd xmm2, xmm0

  ;; Load U0U1 into xmm3 [ U0, U0, U1, U1 ].
  mov ax, [rdx]
  pinsrw xmm3, ax, 0
  punpcklbw xmm3, xmm0
  punpcklwd xmm3, xmm0
  pshufd xmm3, xmm3, 0x50

  ;; Load V0V1 into xmm4 [ V0, V0, V1, V1 ].
  mov ax, [r8]
  pinsrw xmm4, ax, 0
  punpcklbw xmm4, xmm0
  punpcklwd xmm4, xmm0
  pshufd xmm4, xmm4, 0x50

  ;; [ U, U, U, U ] - [ 128, 128, 128, 128 ]
  ;; [ V, V, V, V ] - [ 128, 128, 128, 128 ]
  psubd xmm3, xmm1
  psubd xmm4, xmm1

  ; xmm5 = uv1 = -(1617 * u) - (2378 * v);
  movaps xmm5, xmm3
  pmuldq xmm5, xmm9
  movaps xmm6, xmm4
  pmuldq xmm6, xmm10
  paddd xmm5, xmm6

  ; xmm3 = u1 = (8324 * u);
  pmuldq xmm3, xmm11

  ; xmm4 = v1 = (5727 * v);
  pmuldq xmm4, xmm8

  pslld xmm2, 12            ; y = y << 12

  ; r = yuv_buffer[y_ptr] + v1; (xmm4)
  paddd xmm4, xmm2

  ; g = yuv_buffer[y_ptr] + uv1; (xmm5)
  paddd xmm5, xmm2

  ; b = yuv_buffer[y_ptr] + u1; (xmm3)
  paddd xmm3, xmm2

  psrad xmm4, 12            ; r = r >> 12
  psrad xmm5, 12            ; g = g >> 12
  psrad xmm3, 12            ; b = b >> 12

  pmaxsd xmm4, xmm0        ; if (r < 0) { r = 0; }
  pmaxsd xmm5, xmm0        ; if (g < 0) { g = 0; }
  pmaxsd xmm3, xmm0        ; if (b < 0) { b = 0; }

  pminsd xmm4, xmm15       ; if (r > 255) { r = 255; }
  pminsd xmm5, xmm15       ; if (g > 255) { g = 255; }
  pminsd xmm3, xmm15       ; if (b > 255) { b = 255; }

  pextrb eax, xmm4, 0
  mov [rdi+0], al
  pextrb eax, xmm5, 0
  mov [rdi+1], al
  pextrb eax, xmm3, 0
  mov [rdi+2], al

  pextrb eax, xmm4, 4
  mov [rdi+3], al
  pextrb eax, xmm5, 4
  mov [rdi+4], al
  pextrb eax, xmm3, 4
  mov [rdi+5], al

  pextrb eax, xmm4, 8
  mov [rdi+6], al
  pextrb eax, xmm5, 8
  mov [rdi+7], al
  pextrb eax, xmm3, 8
  mov [rdi+8], al

  pextrb eax, xmm4, 12
  mov [rdi+9], al
  pextrb eax, xmm5, 12
  mov [rdi+10], al
  pextrb eax, xmm3, 12
  mov [rdi+11], al

  add rdi, 12
  add rsi, 4
  add rdx, 2
  add r8, 2
  sub ecx, 4
  jnz yuv422_to_rgb24_int_sse_loop
  ret

test_sse:
  pxor xmm0, xmm0

  ret

