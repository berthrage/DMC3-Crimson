INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixBlisteringSwordsSpread_ReturnAddr1:QWORD
EXTERN g_FixBlisteringSwordsSpread_ReturnAddr2:QWORD

.CODE
; From CPl021Shl01SummonedSword_BlisteringSwords_sub_1401DB1E0:
; dmc3.exe+1DB421 - F3 0F 5C 86 60 01 00 00 - subss xmm0,[rsi+00000160]
FixBlisteringSwordsSpreadDetour1 PROC

    movss xmm1, dword ptr [rsi+160h]
    mulss xmm1, dword ptr [rsi+14h]
    subss xmm0, xmm1
    jmp qword ptr [g_FixBlisteringSwordsSpread_ReturnAddr1]

FixBlisteringSwordsSpreadDetour1 ENDP

; dmc3.exe+1DB440 - F3 0F 10 96 D4 0D 00 00 - movss xmm2,[rsi+00000DD4]
FixBlisteringSwordsSpreadDetour2 PROC

    movss xmm2, dword ptr [rsi+0DD4h]
    mulss xmm2, dword ptr [rsi+14h]
    jmp qword ptr [g_FixBlisteringSwordsSpread_ReturnAddr2]

FixBlisteringSwordsSpreadDetour2 ENDP
END
