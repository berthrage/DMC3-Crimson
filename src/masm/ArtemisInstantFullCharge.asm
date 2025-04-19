.DATA
extern g_ArtemisReworkJumpAddr1:QWORD

.CODE
ArtemisInstantFullChargeDetour1 PROC
    comiss xmm8, xmm1
    jmp qword ptr [g_ArtemisReworkJumpAddr1]

ArtemisInstantFullChargeDetour1 ENDP

.DATA
extern g_ArtemisReworkJumpAddr2:QWORD

.CODE
ArtemisInstantFullChargeDetour2 PROC
    comiss xmm8, xmm0
    jmp qword ptr [g_ArtemisReworkJumpAddr2]

ArtemisInstantFullChargeDetour2 ENDP
END