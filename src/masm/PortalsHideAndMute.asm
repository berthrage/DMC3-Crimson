; HidePortals
.DATA
extern g_PortalsHide_ReturnAddr:QWORD

.CODE
HidePortals PROC 
    cmp rcx, 80h
    je JmpOut
    jmp ActualCode

ActualCode:
    movups xmm0, [rcx]
    movaps xmm2,xmm0

JmpOut:
    jmp qword ptr [g_PortalsHide_ReturnAddr]

HidePortals ENDP

; MutePortals
.DATA
extern g_PortalsMute_ReturnAddr:QWORD

.CODE
PortalsMuteDetour PROC 
    cmp rcx, 80h
    je JmpOut
    jmp ActualCode

ActualCode:
    movups xmm0,[rcx]
    subps xmm0,xmm1

JmpOut:
    jmp qword ptr [g_PortalsMute_ReturnAddr]

PortalsMuteDetour ENDP
END
