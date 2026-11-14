INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixCStageSetHang2_Damping_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang2_Settle_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang2_Gravity_ReturnAddr:QWORD
EXTERN g_FixCStageSetHang2_Lerp_ReturnAddr1:QWORD
EXTERN g_FixCStageSetHang2_Lerp_ReturnAddr2:QWORD
EXTERN g_FixCStageSetHang2_Hit_ReturnAddr1:QWORD
EXTERN g_FixCStageSetHang2_Hit_ReturnAddr2:QWORD
EXTERN g_FixCStageSetHang2_FactorCall:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
dampingBase dd 0.998
settleBase dd 0.95
constOne dd 1.0

.CODE
; From CStageSetHang2_StepPhysics_sub_14025B2E0:
; dmc3.exe+25B5D9 - F3 0F 10 15 1B AB 28 00 - movss xmm2,[dmc3.exe+4E60FC] { (0.998) velocity damping per tick }
FixCStageSetHang2DampingDetour PROC

    sub rsp, 30h
    mov [rsp+20h], rcx ; rcx still holds the tether flag in cl
    movss xmm0, dword ptr [dampingBase]
    movss xmm1, dword ptr [rdi+14h] ; ball speed
    call qword ptr [g_FixCStageSetHang2_FactorCall]
    movaps xmm2, xmm0
    mov rcx, [rsp+20h]
    add rsp, 30h
    jmp qword ptr [g_FixCStageSetHang2_Damping_ReturnAddr]

FixCStageSetHang2DampingDetour ENDP

; From CStageSetHang2_StateSwing_sub_14025D600:
; dmc3.exe+25D619 - F3 0F 10 25 17 F0 26 00 - movss xmm4,[dmc3.exe+4CC638] { (0.95) velocity decay and pull to the anchor per tick }
FixCStageSetHang2SettleDetour PROC

    sub rsp, 50h
    mov [rsp+20h], rcx
    movaps xmmword ptr [rsp+30h], xmm0
    movaps xmmword ptr [rsp+40h], xmm1
    movss xmm0, dword ptr [settleBase]
    movss xmm1, dword ptr [rcx+14h] ; ball speed
    call qword ptr [g_FixCStageSetHang2_FactorCall]
    movaps xmm4, xmm0
    movaps xmm1, xmmword ptr [rsp+40h]
    movaps xmm0, xmmword ptr [rsp+30h]
    mov rcx, [rsp+20h]
    add rsp, 50h
    jmp qword ptr [g_FixCStageSetHang2_Settle_ReturnAddr]

FixCStageSetHang2SettleDetour ENDP

; dmc3.exe+25D639 - F3 0F 5C 05 2B FF 0F 00 - subss xmm0,[dmc3.exe+35D56C] { (1.00) gravity per tick }
FixCStageSetHang2GravityDetour PROC

    subss xmm0, dword ptr [rcx+14h] ; scaled by the ball speed
    jmp qword ptr [g_FixCStageSetHang2_Gravity_ReturnAddr]

FixCStageSetHang2GravityDetour ENDP

; dmc3.exe+25D661 - F3 0F 59 15 17 DC 26 00 - mulss xmm2,[dmc3.exe+4CB280] { (0.05) anchor pull weight X }
FixCStageSetHang2LerpDetour1 PROC

    movss xmm5, dword ptr [constOne]
    subss xmm5, xmm4 ; 1 - decay
    mulss xmm2, xmm5
    jmp qword ptr [g_FixCStageSetHang2_Lerp_ReturnAddr1]

FixCStageSetHang2LerpDetour1 ENDP

; dmc3.exe+25D68D - F3 0F 59 0D EB DB 26 00 - mulss xmm1,[dmc3.exe+4CB280] { (0.05) anchor pull weight Z }
FixCStageSetHang2LerpDetour2 PROC

    movss xmm5, dword ptr [constOne]
    subss xmm5, xmm4
    mulss xmm1, xmm5
    jmp qword ptr [g_FixCStageSetHang2_Lerp_ReturnAddr2]

FixCStageSetHang2LerpDetour2 ENDP

; From CStageSetHang2_OnHit_sub_14025C1D0, the hit impulse is multiplied by the ball speed twice.
; Dividing out the frame scale keeps the impulse the same size at any frame rate.
; dmc3.exe+25C2DD - F3 0F 10 5E B4 - movss xmm3,[rsi-4C]
FixCStageSetHang2HitDetour1 PROC

    movss xmm3, dword ptr [rsi-4Ch]
    movss xmm4, dword ptr [g_FrameRateTimeMultiplier]
    xorps xmm5, xmm5
    ucomiss xmm4, xmm5
    jbe skipDiv1
    divss xmm3, xmm4
skipDiv1:
    jmp qword ptr [g_FixCStageSetHang2_Hit_ReturnAddr1]

FixCStageSetHang2HitDetour1 ENDP

; dmc3.exe+25C34B - F3 0F 10 56 B4 - movss xmm2,[rsi-4C]
FixCStageSetHang2HitDetour2 PROC

    movss xmm2, dword ptr [rsi-4Ch]
    movss xmm4, dword ptr [g_FrameRateTimeMultiplier]
    xorps xmm5, xmm5
    ucomiss xmm4, xmm5
    jbe skipDiv2
    divss xmm2, xmm4
skipDiv2:
    jmp qword ptr [g_FixCStageSetHang2_Hit_ReturnAddr2]

FixCStageSetHang2HitDetour2 ENDP
END
