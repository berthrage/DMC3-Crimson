.DATA
extern g_HudStyleBarPos_ReturnAddr:QWORD
extern g_HudStyleBarPosX:DWORD
extern g_HudStyleBarPosY:DWORD

.CODE
HudStyleBarPosDetour PROC
    addss xmm0, [g_HudStyleBarPosX]
    addss xmm1, [g_HudStyleBarPosY]
    jmp qword ptr [g_HudStyleBarPos_ReturnAddr]

HudStyleBarPosDetour ENDP

END
