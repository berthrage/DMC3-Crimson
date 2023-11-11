.DATA
extern g_EnableAirTaunt_ReturnAddr:QWORD
extern g_EnableAirTaunt_ConditionalAddr:QWORD
extern g_EnableAirTaunt_ConditionalAddr2:QWORD

.CODE
EnableAirTauntDetour PROC
    cmp dword ptr [rdi+78h], 0 ; dante id
    je DanteLabel
    cmp dword ptr [rdi+78h], 3 ; vergil id
    je VergilLabel

DanteLabel:
    ; Disable air taunt when release moveid is active
    cmp byte ptr [rcx+00003fa4h], 194
    je ConditionalLabel
    cmp byte ptr [rcx+00003fa4h], 195
    je ConditionalLabel
    cmp byte ptr [rcx+00003fa4h], 196
    je ConditionalLabel
    cmp byte ptr [rcx+00003fa4h], 197
    je ConditionalLabel
    jmp CheckIfAerialLabel

VergilLabel:
    ; Disable air taunt when ? moveid is active
    cmp byte ptr [rcx+00003fa4h], 25
    je ConditionalLabel
    jmp CheckIfAerialLabel

CheckIfAerialLabel:
    test byte ptr [rcx+00003e64h], 2 ; aerial
    jne ConditionalLabel2
CodeLabel:
    test byte ptr [rcx+00003E64h],01h
    je ConditionalLabel
ReturnLabel:
    jmp qword ptr [g_EnableAirTaunt_ReturnAddr]
ConditionalLabel:
    jmp qword ptr [g_EnableAirTaunt_ConditionalAddr]
ConditionalLabel2:
    jmp qword ptr [g_EnableAirTaunt_ConditionalAddr2]

EnableAirTauntDetour ENDP
END
