.DATA
extern g_VergilNeutralTrick_ReturnAddr:QWORD

.CODE
VergilNeutralTrickDetour PROC
    cmp byte ptr [rax+3e84h], 1 ; locked on
    je CodeLabel
    mov eax, 0000001Ch ; trick up
    jmp ReturnLabel
CodeLabel:
    mov eax, 0000001Bh ; trick
ReturnLabel:
    jmp qword ptr [g_VergilNeutralTrick_ReturnAddr]

VergilNeutralTrickDetour ENDP
END
