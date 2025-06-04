; HidePortals
.DATA
extern g_PortalsHide_ReturnAddr:QWORD

.CODE
PortalsHideDetour PROC 
    cmp eax, 1E7h
    je Replace
    jmp ActualCode

Replace:
    mov eax, 1h
    jmp ActualCode

ActualCode:
    mov [rsp+28h], ax

JmpOut:
    jmp qword ptr [g_PortalsHide_ReturnAddr]

PortalsHideDetour ENDP

; MutePortals
.DATA
extern g_PortalsMute_ReturnAddr:QWORD

.CODE
PortalsMuteDetour PROC 
    cmp eax, 53h
    je Replace
    cmp eax, 52h
    je Replace
    jmp ActualCode

Replace:
    mov eax, 1h
    jmp ActualCode

ActualCode:
    mov [rbx],eax
    lea rbx,[rbx+4h]

JmpOut:
    jmp qword ptr [g_PortalsMute_ReturnAddr]

PortalsMuteDetour ENDP
END
