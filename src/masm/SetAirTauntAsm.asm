.DATA
extern g_SetAirTaunt_ReturnAddr:QWORD
extern g_SetAirTaunt_Call:QWORD

.CODE
SetAirTauntDetour PROC ; player in rcx
    test byte ptr [rcx+3e64h], 2 ; aerial
    je CodeLabel
    mov rdx,0000000000000011h
    cmp dword ptr [rdi+78h], 0 ; dante id
    je DanteLabel
    cmp dword ptr [rdi+78h], 3 ; vergil id
    je VergilLabel
    jmp CodeLabel

DanteLabel:
    mov word ptr [rcx+3fa4h],0c2h
    jmp CodeLabel

VergilLabel:
    mov word ptr [rcx+3fa4h],019h
    jmp CodeLabel

CodeLabel:
    call qword ptr g_SetAirTaunt_Call
ReturnLabel:
    jmp qword ptr [g_SetAirTaunt_ReturnAddr]

SetAirTauntDetour ENDP
END
