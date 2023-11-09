.DATA
extern g_DisableDriveHold_ReturnAddr:QWORD

.CODE
DisableDriveHoldDetour PROC ; player in RDI, rsi holds the offset for each charge
    cmp dword ptr [rdi+78h], 0 ; dante id
    jne CodeLabel
    push rdx
    lea rdx, [rdi+66a4h] ; rebellion charge offset
    cmp rdx, rsi ; is this rebellion's charge input?
    pop rdx
    je ReturnLabel

CodeLabel:
    addss xmm0, dword ptr [rsi+08h]

ReturnLabel:
    jmp qword ptr [g_DisableDriveHold_ReturnAddr]

DisableDriveHoldDetour ENDP
END
