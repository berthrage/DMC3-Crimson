INCLUDE CommonMacros.inc
.DATA
EXTERN g_FrameRateTimeMultiplier:DWORD
EXTERN g_FrameRateTimeMultiplierRounded:DWORD

EXTERN g_FixBlendingEffects_Mist_ReturnAddr1:QWORD
EXTERN g_FixBlendingEffects_Mist_ReturnAddr2:QWORD
EXTERN g_FixBlendingEffects_Mist_ReturnAddr3:QWORD
EXTERN g_MistBlendingEffect_SpdAddr1:QWORD
EXTERN g_MistBlendingEffect_SpdAddr2:QWORD

EXTERN g_FixBlendingEffects_Warp_ReturnAddr1:QWORD
EXTERN g_FixBlendingEffects_Warp_ReturnAddr2:QWORD  

.CODE
FixBlendingEffect_Mist_Detour1 PROC
    push rax
    mov rax, g_MistBlendingEffect_SpdAddr1

    jmp ApplyNewMistSpeed

ApplyNewMistSpeed:
    movss xmm1, dword ptr [rax]
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplierRounded] 

    pop rax

    jmp qword ptr [g_FixBlendingEffects_Mist_ReturnAddr1]

FixBlendingEffect_Mist_Detour1 ENDP


.CODE
FixBlendingEffect_Mist_Detour2 PROC
    push rax
    mov rax, g_MistBlendingEffect_SpdAddr2

    jmp ApplyNewMistSpeed

ApplyNewMistSpeed:
    mulss xmm0, dword ptr [rax]
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplierRounded] 

    pop rax

    jmp qword ptr [g_FixBlendingEffects_Mist_ReturnAddr2]

FixBlendingEffect_Mist_Detour2 ENDP


.CODE
FixBlendingEffect_Mist_Detour3 PROC
    push rax
    mov rax, g_MistBlendingEffect_SpdAddr2

    jmp ApplyNewMistSpeed

ApplyNewMistSpeed:
    mulss xmm6, dword ptr [rax]
    mulss xmm6, dword ptr [g_FrameRateTimeMultiplierRounded] 

    pop rax

    jmp qword ptr [g_FixBlendingEffects_Mist_ReturnAddr3]

FixBlendingEffect_Mist_Detour3 ENDP


.CODE
FixBlendingEffect_Warp_Detour1 PROC
    sub rsp, 20h ; Reserving shadow-space for xmm
    movdqu [rsp], xmm1 ; Save xmm1

    mov eax, [rsi+20h]
    cvtsi2ss  xmm1, eax ; Convert the turn rate value to float in xmm1
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]

    jmp ApplyNewWarpSpeed

ApplyNewWarpSpeed:
    cvttss2si eax, xmm1
    mov edx,ebx

    movdqu xmm1, [rsp]
    add rsp, 20h
    jmp qword ptr [g_FixBlendingEffects_Warp_ReturnAddr1]

FixBlendingEffect_Warp_Detour1 ENDP


.CODE
FixBlendingEffect_Warp_Detour2 PROC
    sub rsp, 20h ; Reserving shadow-space for xmm
    movdqu [rsp], xmm1 ; Save xmm1

    mov eax, [rsi+24h]
    cvtsi2ss  xmm1, eax ; Convert the turn rate value to float in xmm1
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]

    jmp ApplyNewWarpSpeed

ApplyNewWarpSpeed:
    cvttss2si eax, xmm1
    add [rsi+74h],eax

    movdqu xmm1, [rsp]
    add rsp, 20h
    jmp qword ptr [g_FixBlendingEffects_Warp_ReturnAddr2]

FixBlendingEffect_Warp_Detour2 ENDP

END
