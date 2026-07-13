INCLUDE CommonMacros.inc
.DATA
EXTERN g_EnsureAirRisingDragonLaunch_ReturnAddr:QWORD
EXTERN g_EnsureAirRisingDragonLaunch_JmpAddr:QWORD
EXTERN g_AdjustAirRisingDragonLaunchHeight_ReturnAddr:QWORD
EXTERN g_AdjustAirRisingDragonLaunchHeight_FuncCall:QWORD
newYinertia dd 10.0f

.CODE
EnsureAirRisingDragonLaunchDetour PROC
    ; player in rbx
    push r13
    mov r13, [rbx+3e64h] ; check if player is in air state
    test r13, 2
    pop r13
    je OriginalCode
    jmp g_EnsureAirRisingDragonLaunch_JmpAddr

OriginalCode:
    cmp byte ptr [rbx+3fa7h], 00      
    jmp qword ptr [g_EnsureAirRisingDragonLaunch_ReturnAddr]

EnsureAirRisingDragonLaunchDetour ENDP

.CODE
AdjustAirRisingDragonLaunchHeightDetour PROC
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
    call qword ptr [g_AdjustAirRisingDragonLaunchHeight_FuncCall]
    add rsp, 28h
    movss xmm2, xmm0
    PopAllXmmExcept xmm2
    PopAllRegs
    jmp qword ptr [g_AdjustAirRisingDragonLaunchHeight_ReturnAddr]

OriginalCode:
    movss xmm2, dword ptr [rax + 027Ch]
    jmp qword ptr [g_AdjustAirRisingDragonLaunchHeight_ReturnAddr]

AdjustAirRisingDragonLaunchHeightDetour ENDP
END