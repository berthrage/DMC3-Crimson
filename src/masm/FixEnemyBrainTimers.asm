INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixEnemyZoneGauge_ReturnAddr:QWORD
EXTERN g_FixEnemyDTGauge_ReturnAddr:QWORD
EXTERN g_FixEnemyActionWait_ReturnAddr:QWORD
EXTERN g_FixEnemyActionSlide_ReturnAddr:QWORD
EXTERN g_EnemySpeedChannelAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

.CODE
; From CComZoneGaugeCheck_sub_140060CA0:
; dmc3.exe+60DA8 - F3 0F 10 05 F0 1F C9 00 - movss xmm0,[dmc3.exe+CF2DA0] { Enemy speed channel }
FixEnemyZoneGaugeDetour PROC

    mov rax, qword ptr [g_EnemySpeedChannelAddr]
    movss xmm0, dword ptr [rax]
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier]
    jmp qword ptr [g_FixEnemyZoneGauge_ReturnAddr]

FixEnemyZoneGaugeDetour ENDP

; From CComUpdateDTGauge_sub_140061000:
; dmc3.exe+61006 - F3 0F 10 0D 92 1D C9 00 - movss xmm1,[dmc3.exe+CF2DA0] { Enemy speed channel }
FixEnemyDTGaugeDetour PROC

    mov rax, qword ptr [g_EnemySpeedChannelAddr]
    movss xmm1, dword ptr [rax]
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]
    jmp qword ptr [g_FixEnemyDTGauge_ReturnAddr]

FixEnemyDTGaugeDetour ENDP

; From CComActionWaitSlide_sub_140061AB0:
; dmc3.exe+61BB7 - F3 0F 5C 0D AD B9 2F 00 - subss xmm1,[dmc3.exe+35D56C] { 1.0, wait ticks countdown }
FixEnemyActionWaitDetour PROC

    subss xmm1, dword ptr [g_FrameRateTimeMultiplier]
    jmp qword ptr [g_FixEnemyActionWait_ReturnAddr]

FixEnemyActionWaitDetour ENDP

; From CComActionWaitSlide_sub_140061AB0:
; dmc3.exe+61B41 - 0F 28 43 50 - movaps xmm0,[rbx+50] { per tick velocity }
; dmc3.exe+61B45 - 0F 58 43 40 - addps xmm0,[rbx+40]
FixEnemyActionSlideDetour PROC

    movaps xmm0, xmmword ptr [rbx+50h]
    movss xmm1, dword ptr [g_FrameRateTimeMultiplier]
    shufps xmm1, xmm1, 0
    mulps xmm0, xmm1
    addps xmm0, xmmword ptr [rbx+40h]
    jmp qword ptr [g_FixEnemyActionSlide_ReturnAddr]

FixEnemyActionSlideDetour ENDP
END
