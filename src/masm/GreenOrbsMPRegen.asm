INCLUDE CommonMacros.inc
.DATA
extern g_GreenOrbsMPRegen_ReturnAddr:QWORD
extern g_GreenOrbsMPRegen_Call:QWORD

; Temporary storage for the floating-point value
align 16
float_temp dq 0.0

.CODE
GreenOrbsMPRegenDetour PROC
    ; player in RDX
    PushAllXmm
    PushAllRegs

    movss dword ptr [float_temp], xmm0
   
    ; Load the float value from the stack into rcx (interpreting it as a uint64_t)
    mov rax, qword ptr [float_temp]
    mov rcx, rax              ; Copy full 64-bit value into RCX

    ; Call the C++ function to transfer HP Amount to All other players
    call qword ptr [g_GreenOrbsMPRegen_Call]


    jmp ContinueOriginalCode

ContinueOriginalCode:
    PopAllRegs
    PopAllXmm

    movss xmm0, dword ptr [float_temp]
    addss xmm0, dword ptr [rdx+411Ch]
    jmp qword ptr [g_GreenOrbsMPRegen_ReturnAddr]
    
GreenOrbsMPRegenDetour ENDP

END
