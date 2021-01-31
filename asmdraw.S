.arch armv4t

.section .iwram, "ax", %progbits
.align 4

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

@ takes 20k cycles
@ r0 - vid addr start
.global fast_clear
fast_clear:
    push {r4-r12}
    push {r14}
    mov r1, #0
    mov r2, #0
    mov r3, #0
    mov r4, #0
    mov r5, #0
    mov r6, #0
    mov r7, #0
    mov r8, #0
    mov r9, #0
    mov r10, #0
    mov r11, #0
    mov r12, #0
    mov r14, #0x14
    1:
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        stmia r0!, {r1-r12}
        subs r14, #1
        bne 1b
    
    pop {r14}
    pop {r4-r12}
    bx lr
