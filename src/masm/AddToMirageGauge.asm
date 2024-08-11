.DATA
extern g_AddToMirageGauge_ReturnAddr:QWORD
extern g_AddToMirageGaugeCall:QWORD

; Temporary storage for the floating-point value
align 16
float_temp dq 0.0

.CODE
AddToMirageGaugeDetour PROC
    ; Preserve xmm1 and xmm0
    movss xmm12, xmm1
    movss xmm13, xmm0
    
    ; Preserve player in r14 because the call will mess with rcx
    mov r14, rcx

    ; Align the stack
    sub rsp, 64           ; Adjust the stack for alignment

    ; Move the float in xmm1 to memory (float_temp)
    movss dword ptr [float_temp], xmm1

    ; Load the float value from memory into rdx (interpreting it as a uint64_t)
    mov rdx, qword ptr [float_temp]

    ; Call the C++ function to transfer DT amount to MirageGauge
    call qword ptr [g_AddToMirageGaugeCall]

    ; Restore the stack
    add rsp, 64           ; Restore stack after the call

    jmp ContinueOriginalCode

ContinueOriginalCode:
    ; Restore xmm1 and xmm0
    movss xmm1, xmm12
    movss xmm0, xmm13
    mov rcx, r14
    cmp byte ptr [rcx + 3E98h], 1
    jmp qword ptr [g_AddToMirageGauge_ReturnAddr]
    
AddToMirageGaugeDetour ENDP

END
