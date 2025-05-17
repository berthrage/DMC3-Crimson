.DATA
extern g_GreenOrbsMPRegen_ReturnAddr:QWORD
extern g_GreenOrbsMPRegen_Call:QWORD

; Temporary storage for the floating-point value
align 16
float_temp dq 0.0

.CODE
GreenOrbsMPRegenDetour PROC
    ; player in RDX
    push    rax
    push    rcx
    push    rdx
    push    r8
    push    r9
    push    r10
    push    r11
    push    rbx
    push    rdi

    ; Move the float in xmm1 to memory (float_temp) to preserve it
    ;movss dword ptr [float_temp], xmm0
    sub     rsp, 48h          ; Reserve space for xmm0, xmm2, xmm6
    movdqu  [rsp+00h], xmm0
    movdqu  [rsp+10h], xmm2
    movdqu  [rsp+20h], xmm6

    movss dword ptr [float_temp], xmm0
   
    ; Load the float value from the stack into rcx (interpreting it as a uint64_t)
    mov rax, qword ptr [float_temp]
    mov rcx, rax              ; Copy full 64-bit value into RCX

    ; Call the C++ function to transfer HP Amount to All other players
    call qword ptr [g_GreenOrbsMPRegen_Call]


    jmp ContinueOriginalCode

ContinueOriginalCode:
 ; Restore xmm registers
    movdqu  xmm0, [rsp+00h]
    movdqu  xmm2, [rsp+10h]
    movdqu  xmm6, [rsp+20h]
    add     rsp, 48h

    ; Restore registers
    pop     rdi
    pop     rbx
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rdx
    pop     rcx
    pop     rax

    movss xmm0, dword ptr [float_temp]
    addss xmm0, dword ptr [rdx+411Ch]
    jmp qword ptr [g_GreenOrbsMPRegen_ReturnAddr]
    
GreenOrbsMPRegenDetour ENDP

END
