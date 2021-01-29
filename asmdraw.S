.arch armv4t

.section .iwram, "ax", %progbits
.align 2

; @ r0 - start address
; @ r1 - num of pixels
; @ r2 - color
; .global fill_scanline
; fill_scanline:

;     .loop:
;         str r2, [r0], #4
;         subs r1, #4
;         bne .loop

;     bx lr

@ r0 - addr
@ r1 - col
.global plot_pixel
plot_pixel:
    tst r0, #1
    bic r0, #1
    ldrh r2, [r0] @ prev value

    bne .odd
        @even
        bic r2, #0xFF
        orr r2, r1
        b .done
    .odd:
        bic r2, #0xFF00
        orr r2, r1, LSL #8
    .done:

    strh r2, [r0]
    bx lr
