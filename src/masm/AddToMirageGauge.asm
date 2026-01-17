INCLUDE CommonMacros.inc
.DATA
extern g_AddToMirageGauge_ReturnAddr:QWORD
extern g_AddToMirageGaugeCall:QWORD

; Temporary storage for the floating-point value
align 16
float_temp dq 0.0
saved_rcx dq 0

.CODE
AddToMirageGaugeDetour PROC
    PushAllRegsAndXmm
    movss dword ptr [float_temp], xmm1
    ; Align the stack
    sub rsp, 64           ; Adjust the stack for alignment

    ; Load the float value from memory into rdx (interpreting it as a uint64_t)
    mov rdx, qword ptr [float_temp]

    ; Call the C++ function to transfer DT amount to MirageGauge
    call qword ptr [g_AddToMirageGaugeCall]

    ; Restore the stack
    add rsp, 64           ; Restore stack after the call

    jmp ContinueOriginalCode

ContinueOriginalCode:
    ; Restore xmm1 and xmm0
    PopAllRegsAndXmm
    cmp byte ptr [rcx + 3E9Bh], 1
    jmp qword ptr [g_AddToMirageGauge_ReturnAddr]
    
AddToMirageGaugeDetour ENDP

END
