INCLUDE CommonMacros.inc

.DATA
EXTERN g_AdjustAirWhirlwindLaunchHeight_ReturnAddr:QWORD
EXTERN g_AdjustAirWhirlwindLaunchHeightCheckCall:QWORD     

.CODE
; From ExecuteWhirlwindLaunchMove_sub_1401FFFF0:
; dmc3.exe+2000E5 - F3 0F 10 90 50 03 00 00 - movss xmm2,[rax+00000350] { new y inertia to be applied }
AdjustAirWhirlwindLaunchHeightDetour PROC
    ; player in rbx
    ; charsettings in rax
    push r13
    mov r13, [rbx+3e64h] ; check if player is in air state
    test r13, 2
    pop r13
    je OriginalCode
    PushAllRegs
    PushAllXmmExcept xmm2
    mov rcx, rbx
    sub rsp, 28h ; Reserving shadow-space for the call
    call qword ptr [g_AdjustAirWhirlwindLaunchHeightCheckCall]
    add rsp, 28h
    movss xmm2, xmm0
    PopAllXmmExcept xmm2
    PopAllRegs
    jmp qword ptr [g_AdjustAirWhirlwindLaunchHeight_ReturnAddr]

OriginalCode:
    movss xmm2, dword ptr [rax + 0350h]
    jmp qword ptr [g_AdjustAirWhirlwindLaunchHeight_ReturnAddr]


AdjustAirWhirlwindLaunchHeightDetour ENDP
END