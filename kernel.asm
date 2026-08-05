; Kent Lopez   S25F

; x86-64 NASM implementation of:
;   Z[i] = sqrt( (X2[i]-X1[i])^2 + (Y2[i]-Y1[i])^2 )
;
; Windows x64 calling convention parameters mapping:
;   RCX = size_t n            (1st param)
;   RDX = const float* x1     (2nd param)
;   R8  = const float* x2     (3rd param)
;   R9  = const float* y1     (4th param)
;   [rbp+32] = const float* y2 (5th param, on stack past shadow space)
;   [rbp+40] = float* z        (6th param, on stack)

section .text
bits 64
default rel

global distance_asm

distance_asm:
    push rbp
    mov  rbp, rsp
    add  rbp, 16        ; account for pushed rbp (+8) and return address (+8)

    mov  r11, [rbp+32]  ; r11 = y2 pointer (5th parameter)
    mov  rax, [rbp+40]  ; rax = z pointer (6th parameter)
    xor  r10, r10       ; r10 = loop index i = 0

.loop:
    cmp  r10, rcx       ; compare index i (r10) with n (rcx)
    jge  .done

    ; --- dx = X2[i] - X1[i]; dx = dx*dx ---
    movss xmm0, [r8 + r10*4]   ; xmm0 = X2[i]
    subss xmm0, [rdx + r10*4]  ; xmm0 = X2[i] - X1[i]
    mulss xmm0, xmm0           ; xmm0 = dx^2

    ; --- dy = Y2[i] - Y1[i]; dy = dy*dy ---
    movss xmm1, [r11 + r10*4]  ; xmm1 = Y2[i]
    subss xmm1, [r9 + r10*4]   ; xmm1 = Y2[i] - Y1[i]
    mulss xmm1, xmm1           ; xmm1 = dy^2

    ; --- Z[i] = sqrt(dx^2 + dy^2) ---
    addss xmm0, xmm1           ; xmm0 = dx^2 + dy^2
    sqrtss xmm0, xmm0          ; xmm0 = sqrt(dx^2 + dy^2)
    movss [rax + r10*4], xmm0  ; Z[i] = xmm0

    inc  r10
    jmp  .loop

.done:
    pop  rbp
    ret