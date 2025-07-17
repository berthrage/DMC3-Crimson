; CommonMacros.inc
PushAllRegs MACRO
    pushfq
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
ENDM

PopAllRegs MACRO
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
    popfq
ENDM

PushAllRegsAndXmm MACRO
    ; Save XMM registers first (assume rsp is 16-byte aligned here)
    sub     rsp, 100h            ; 16 * 16 bytes
    movdqu  [rsp+00h],   xmm0
    movdqu  [rsp+10h],   xmm1
    movdqu  [rsp+20h],   xmm2
    movdqu  [rsp+30h],   xmm3
    movdqu  [rsp+40h],   xmm4
    movdqu  [rsp+50h],   xmm5
    movdqu  [rsp+60h],   xmm6
    movdqu  [rsp+70h],   xmm7
    movdqu  [rsp+80h],   xmm8
    movdqu  [rsp+90h],   xmm9
    movdqu  [rsp+0A0h],  xmm10
    movdqu  [rsp+0B0h],  xmm11
    movdqu  [rsp+0C0h],  xmm12
    movdqu  [rsp+0D0h],  xmm13
    movdqu  [rsp+0E0h],  xmm14
    movdqu  [rsp+0F0h],  xmm15

    ; Save GP registers (same as PushAllRegs)
    pushfq
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
ENDM


PopAllRegsAndXmm MACRO
    ; Restore GP registers
    pop rdi
    pop rbx
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
    popfq

    ; Restore XMM registers
    movdqu  xmm0,  [rsp+00h]
    movdqu  xmm1,  [rsp+10h]
    movdqu  xmm2,  [rsp+20h]
    movdqu  xmm3,  [rsp+30h]
    movdqu  xmm4,  [rsp+40h]
    movdqu  xmm5,  [rsp+50h]
    movdqu  xmm6,  [rsp+60h]
    movdqu  xmm7,  [rsp+70h]
    movdqu  xmm8,  [rsp+80h]
    movdqu  xmm9,  [rsp+90h]
    movdqu  xmm10, [rsp+0A0h]
    movdqu  xmm11, [rsp+0B0h]
    movdqu  xmm12, [rsp+0C0h]
    movdqu  xmm13, [rsp+0D0h]
    movdqu  xmm14, [rsp+0E0h]
    movdqu  xmm15, [rsp+0F0h]

    ; Clean up XMM temp stack space
    add     rsp, 100h
ENDM

PopAllRegsAndXmm MACRO
    ; Restore GP registers in reverse order (same as PopAllRegs)
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
    popfq

    ; Restore XMM registers
    movdqu  xmm0,  [rsp+00h]
    movdqu  xmm1,  [rsp+10h]
    movdqu  xmm2,  [rsp+20h]
    movdqu  xmm3,  [rsp+30h]
    movdqu  xmm4,  [rsp+40h]
    movdqu  xmm5,  [rsp+50h]
    movdqu  xmm6,  [rsp+60h]
    movdqu  xmm7,  [rsp+70h]
    movdqu  xmm8,  [rsp+80h]
    movdqu  xmm9,  [rsp+90h]
    movdqu  xmm10, [rsp+0A0h]
    movdqu  xmm11, [rsp+0B0h]
    movdqu  xmm12, [rsp+0C0h]
    movdqu  xmm13, [rsp+0D0h]
    movdqu  xmm14, [rsp+0E0h]
    movdqu  xmm15, [rsp+0F0h]

    ; Clean up XMM temp stack space
    add     rsp, 100h
ENDM

PushAllXmm MACRO
    sub     rsp, 100h
    movdqu  [rsp + 00h], xmm0
    movdqu  [rsp + 10h], xmm1
    movdqu  [rsp + 20h], xmm2
    movdqu  [rsp + 30h], xmm3
    movdqu  [rsp + 40h], xmm4
    movdqu  [rsp + 50h], xmm5
    movdqu  [rsp + 60h], xmm6
    movdqu  [rsp + 70h], xmm7
    movdqu  [rsp + 80h], xmm8
    movdqu  [rsp + 90h], xmm9
    movdqu  [rsp + 0A0h], xmm10
    movdqu  [rsp + 0B0h], xmm11
    movdqu  [rsp + 0C0h], xmm12
    movdqu  [rsp + 0D0h], xmm13
    movdqu  [rsp + 0E0h], xmm14
    movdqu  [rsp + 0F0h], xmm15
ENDM

PopAllXmm MACRO
    movdqu  xmm15, [rsp + 0F0h]
    movdqu  xmm14, [rsp + 0E0h]
    movdqu  xmm13, [rsp + 0D0h]
    movdqu  xmm12, [rsp + 0C0h]
    movdqu  xmm11, [rsp + 0B0h]
    movdqu  xmm10, [rsp + 0A0h]
    movdqu  xmm9,  [rsp + 90h]
    movdqu  xmm8,  [rsp + 80h]
    movdqu  xmm7,  [rsp + 70h]
    movdqu  xmm6,  [rsp + 60h]
    movdqu  xmm5,  [rsp + 50h]
    movdqu  xmm4,  [rsp + 40h]
    movdqu  xmm3,  [rsp + 30h]
    movdqu  xmm2,  [rsp + 20h]
    movdqu  xmm1,  [rsp + 10h]
    movdqu  xmm0,  [rsp + 00h]
    add     rsp, 100h
ENDM

PushAllXmmExcept MACRO skipReg:REQ
    sub     rsp, 100h
    IFIDNI <skipReg>, <xmm0>
    ELSE
        movdqu  [rsp + 00h], xmm0
    ENDIF
    IFIDNI <skipReg>, <xmm1>
    ELSE
        movdqu  [rsp + 10h], xmm1
    ENDIF
    IFIDNI <skipReg>, <xmm2>
    ELSE
        movdqu  [rsp + 20h], xmm2
    ENDIF
    IFIDNI <skipReg>, <xmm3>
    ELSE
        movdqu  [rsp + 30h], xmm3
    ENDIF
    IFIDNI <skipReg>, <xmm4>
    ELSE
        movdqu  [rsp + 40h], xmm4
    ENDIF
    IFIDNI <skipReg>, <xmm5>
    ELSE
        movdqu  [rsp + 50h], xmm5
    ENDIF
    IFIDNI <skipReg>, <xmm6>
    ELSE
        movdqu  [rsp + 60h], xmm6
    ENDIF
    IFIDNI <skipReg>, <xmm7>
    ELSE
        movdqu  [rsp + 70h], xmm7
    ENDIF
    IFIDNI <skipReg>, <xmm8>
    ELSE
        movdqu  [rsp + 80h], xmm8
    ENDIF
    IFIDNI <skipReg>, <xmm9>
    ELSE
        movdqu  [rsp + 90h], xmm9
    ENDIF
    IFIDNI <skipReg>, <xmm10>
    ELSE
        movdqu  [rsp + 0A0h], xmm10
    ENDIF
    IFIDNI <skipReg>, <xmm11>
    ELSE
        movdqu  [rsp + 0B0h], xmm11
    ENDIF
    IFIDNI <skipReg>, <xmm12>
    ELSE
        movdqu  [rsp + 0C0h], xmm12
    ENDIF
    IFIDNI <skipReg>, <xmm13>
    ELSE
        movdqu  [rsp + 0D0h], xmm13
    ENDIF
    IFIDNI <skipReg>, <xmm14>
    ELSE
        movdqu  [rsp + 0E0h], xmm14
    ENDIF
    IFIDNI <skipReg>, <xmm15>
    ELSE
        movdqu  [rsp + 0F0h], xmm15
    ENDIF
ENDM

PopAllXmmExcept MACRO skipReg:REQ
    IFIDNI <skipReg>, <xmm15>
    ELSE
        movdqu  xmm15, [rsp + 0F0h]
    ENDIF
    IFIDNI <skipReg>, <xmm14>
    ELSE
        movdqu  xmm14, [rsp + 0E0h]
    ENDIF
    IFIDNI <skipReg>, <xmm13>
    ELSE
        movdqu  xmm13, [rsp + 0D0h]
    ENDIF
    IFIDNI <skipReg>, <xmm12>
    ELSE
        movdqu  xmm12, [rsp + 0C0h]
    ENDIF
    IFIDNI <skipReg>, <xmm11>
    ELSE
        movdqu  xmm11, [rsp + 0B0h]
    ENDIF
    IFIDNI <skipReg>, <xmm10>
    ELSE
        movdqu  xmm10, [rsp + 0A0h]
    ENDIF
    IFIDNI <skipReg>, <xmm9>
    ELSE
        movdqu  xmm9, [rsp + 90h]
    ENDIF
    IFIDNI <skipReg>, <xmm8>
    ELSE
        movdqu  xmm8, [rsp + 80h]
    ENDIF
    IFIDNI <skipReg>, <xmm7>
    ELSE
        movdqu  xmm7, [rsp + 70h]
    ENDIF
    IFIDNI <skipReg>, <xmm6>
    ELSE
        movdqu  xmm6, [rsp + 60h]
    ENDIF
    IFIDNI <skipReg>, <xmm5>
    ELSE
        movdqu  xmm5, [rsp + 50h]
    ENDIF
    IFIDNI <skipReg>, <xmm4>
    ELSE
        movdqu  xmm4, [rsp + 40h]
    ENDIF
    IFIDNI <skipReg>, <xmm3>
    ELSE
        movdqu  xmm3, [rsp + 30h]
    ENDIF
    IFIDNI <skipReg>, <xmm2>
    ELSE
        movdqu  xmm2, [rsp + 20h]
    ENDIF
    IFIDNI <skipReg>, <xmm1>
    ELSE
        movdqu  xmm1, [rsp + 10h]
    ENDIF
    IFIDNI <skipReg>, <xmm0>
    ELSE
        movdqu  xmm0, [rsp + 00h]
    ENDIF
    add     rsp, 100h
ENDM

PushAllRegsExcept MACRO skipReg:REQ
    pushfq
    IFIDNI <skipReg>, <rax>
    ELSE
        push rax
    ENDIF
    IFIDNI <skipReg>, <rcx>
    ELSE
        push rcx
    ENDIF
    IFIDNI <skipReg>, <rdx>
    ELSE
        push rdx
    ENDIF
    IFIDNI <skipReg>, <rbx>
    ELSE
        push rbx
    ENDIF
    IFIDNI <skipReg>, <rbp>
    ELSE
        push rbp
    ENDIF
    IFIDNI <skipReg>, <rsi>
    ELSE
        push rsi
    ENDIF
    IFIDNI <skipReg>, <rdi>
    ELSE
        push rdi
    ENDIF
    IFIDNI <skipReg>, <r8>
    ELSE
        push r8
    ENDIF
    IFIDNI <skipReg>, <r9>
    ELSE
        push r9
    ENDIF
    IFIDNI <skipReg>, <r10>
    ELSE
        push r10
    ENDIF
    IFIDNI <skipReg>, <r11>
    ELSE
        push r11
    ENDIF
    IFIDNI <skipReg>, <r12>
    ELSE
        push r12
    ENDIF
    IFIDNI <skipReg>, <r13>
    ELSE
        push r13
    ENDIF
    IFIDNI <skipReg>, <r14>
    ELSE
        push r14
    ENDIF
    IFIDNI <skipReg>, <r15>
    ELSE
        push r15
    ENDIF
ENDM


PopAllRegsExcept MACRO skipReg:REQ
    IFIDNI <skipReg>, <r15>
    ELSE
        pop r15
    ENDIF
    IFIDNI <skipReg>, <r14>
    ELSE
        pop r14
    ENDIF
    IFIDNI <skipReg>, <r13>
    ELSE
        pop r13
    ENDIF
    IFIDNI <skipReg>, <r12>
    ELSE
        pop r12
    ENDIF
    IFIDNI <skipReg>, <r11>
    ELSE
        pop r11
    ENDIF
    IFIDNI <skipReg>, <r10>
    ELSE
        pop r10
    ENDIF
    IFIDNI <skipReg>, <r9>
    ELSE
        pop r9
    ENDIF
    IFIDNI <skipReg>, <r8>
    ELSE
        pop r8
    ENDIF
    IFIDNI <skipReg>, <rdi>
    ELSE
        pop rdi
    ENDIF
    IFIDNI <skipReg>, <rsi>
    ELSE
        pop rsi
    ENDIF
    IFIDNI <skipReg>, <rbp>
    ELSE
        pop rbp
    ENDIF
    IFIDNI <skipReg>, <rbx>
    ELSE
        pop rbx
    ENDIF
    IFIDNI <skipReg>, <rdx>
    ELSE
        pop rdx
    ENDIF
    IFIDNI <skipReg>, <rcx>
    ELSE
        pop rcx
    ENDIF
    IFIDNI <skipReg>, <rax>
    ELSE
        pop rax
    ENDIF
    popfq
ENDM

