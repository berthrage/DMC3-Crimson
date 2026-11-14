INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixCStageSetHang1_Damping_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang1_Settle_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang1_Gravity_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang1_Lerp_ReturnAddr1:QWORD
EXTERN g_FixCStageSetHang1_Lerp_ReturnAddr2:QWORD
EXTERN g_FixCStageSetHang1_Hit_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang1_Rise_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang1_FactorCall:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
dampingBase dd 0.998
settleBase dd 0.95
constOne dd 1.0
riseStep dd 10.0

.CODE
; From CStageSetHang1_StepPhysics_sub_140258AE0:
; dmc3.exe+258D28 - F3 0F 10 15 CC D3 28 00 - movss xmm2,[dmc3.exe+4E60FC] { (0.998) velocity damping per tick }
FixCStageSetHang1DampingDetour PROC

    sub rsp, 30h
    mov [rsp+20h], rcx
    mov [rsp+28h], rdx ; dl holds the tether flag
    movss xmm0, dword ptr [dampingBase]
    movss xmm1, dword ptr [rdi+14h] ; ball speed
    call qword ptr [g_FixCStageSetHang1_FactorCall]
    movaps xmm2, xmm0
    mov rdx, [rsp+28h]
    mov rcx, [rsp+20h]
    add rsp, 30h
    jmp qword ptr [g_FixCStageSetHang1_Damping_ReturnAddr]

FixCStageSetHang1DampingDetour ENDP

; From CStageSetHang1_StateSwing_sub_14025ADB0, which has no prologue so the stack is 8 off at entry:
; dmc3.exe+25ADC3 - F3 0F 10 25 6D 18 27 00 - movss xmm4,[dmc3.exe+4CC638] { (0.95) velocity decay and pull to the anchor per tick }
FixCStageSetHang1SettleDetour PROC

    sub rsp, 58h
    mov [rsp+20h], rcx
    mov [rsp+28h], r8 ; holds the ball for the rest of the state function
    movaps xmmword ptr [rsp+30h], xmm0
    movaps xmmword ptr [rsp+40h], xmm1
    movss xmm0, dword ptr [settleBase]
    movss xmm1, dword ptr [rcx+14h] ; ball speed
    call qword ptr [g_FixCStageSetHang1_FactorCall]
    movaps xmm4, xmm0
    movaps xmm1, xmmword ptr [rsp+40h]
    movaps xmm0, xmmword ptr [rsp+30h]
    mov r8, [rsp+28h]
    mov rcx, [rsp+20h]
    add rsp, 58h
    jmp qword ptr [g_FixCStageSetHang1_Settle_ReturnAddr]

FixCStageSetHang1SettleDetour ENDP

; dmc3.exe+25ADE3 - F3 0F 5C 05 81 27 10 00 - subss xmm0,[dmc3.exe+35D56C] { (1.00) gravity per tick }
FixCStageSetHang1GravityDetour PROC

    subss xmm0, dword ptr [rcx+14h] ; scaled by the ball speed
    jmp qword ptr [g_FixCStageSetHang1_Gravity_ReturnAddr]

FixCStageSetHang1GravityDetour ENDP

; dmc3.exe+25AE0B - F3 0F 59 15 6D 04 27 00 - mulss xmm2,[dmc3.exe+4CB280] { (0.05) anchor pull weight X }
FixCStageSetHang1LerpDetour1 PROC

    movss xmm5, dword ptr [constOne]
    subss xmm5, xmm4 ; 1 - decay
    mulss xmm2, xmm5
    jmp qword ptr [g_FixCStageSetHang1_Lerp_ReturnAddr1]

FixCStageSetHang1LerpDetour1 ENDP

; dmc3.exe+25AE37 - F3 0F 59 0D 41 04 27 00 - mulss xmm1,[dmc3.exe+4CB280] { (0.05) anchor pull weight Z }
FixCStageSetHang1LerpDetour2 PROC

    movss xmm5, dword ptr [constOne]
    subss xmm5, xmm4
    mulss xmm1, xmm5
    jmp qword ptr [g_FixCStageSetHang1_Lerp_ReturnAddr2]

FixCStageSetHang1LerpDetour2 ENDP

; From CStageSetHang1_OnHit_sub_140259930, the hit impulse is multiplied by the ball speed twice.
; Dividing out the frame scale keeps the impulse the same size at any frame rate.
; dmc3.exe+25999F - F3 0F 10 77 B4 - movss xmm6,[rdi-4C]
FixCStageSetHang1HitDetour PROC

    movss xmm6, dword ptr [rdi-4Ch]
    movss xmm4, dword ptr [g_FrameRateTimeMultiplier]
    xorps xmm5, xmm5
    ucomiss xmm4, xmm5
    jbe skipDiv
    divss xmm6, xmm4
skipDiv:
    jmp qword ptr [g_FixCStageSetHang1_Hit_ReturnAddr]

FixCStageSetHang1HitDetour ENDP

; From CStageSetHang1_StateBroken_sub_140259BF0:
; dmc3.exe+259C16 - F3 0F 58 0D 22 C3 26 00 - addss xmm1,[dmc3.exe+4C5F40] { (10.0) anchor rise per tick }
FixCStageSetHang1RiseDetour PROC

    movss xmm0, dword ptr [rcx+14h]
    mulss xmm0, dword ptr [riseStep] ; scaled by the ball speed
    addss xmm1, xmm0
    jmp qword ptr [g_FixCStageSetHang1_Rise_ReturnAddr]

FixCStageSetHang1RiseDetour ENDP
END
