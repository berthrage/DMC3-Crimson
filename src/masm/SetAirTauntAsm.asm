INCLUDE CommonMacros.inc
.DATA
extern g_SetAirTaunt_ReturnAddr:QWORD
extern g_SetAirTaunt_Call:QWORD
extern g_SetAirTauntEcstasyCheckCall:QWORD
extern g_SetAirTauntATRisingSunCheckCall:QWORD

.CODE
SetAirTauntDetour PROC ; player in rcx
    test byte ptr [rcx+3e64h], 2 ; aerial
    je CodeLabel
    cmp dword ptr [rcx+3e00h], 44d ; event staggered
    je ReturnLabel
    mov rdx,0000000000000011h
    cmp dword ptr [rdi+78h], 0 ; dante id
    je DanteLabel
    cmp dword ptr [rdi+78h], 3 ; vergil id
    je VergilLabel
    jmp CodeLabel

DanteLabel:
    PushAllRegsNoFlags
    PushAllXmm
    mov rbp, rsp ; aligned stack + shadow space for the C++ call
    and rsp, -16
    sub rsp, 20h
    call qword ptr [g_SetAirTauntEcstasyCheckCall]
    mov rsp, rbp
    PopAllXmm
    cmp al, 1
    PopAllRegsNoFlags
    je ApplyEcstasy
    jmp ReturnLabel
    

ApplyEcstasy:
    mov word ptr [rcx+3fa4h],040d ; cerb flicker
    jmp CodeLabel

VergilLabel:
    PushAllRegsNoFlags
    PushAllXmm
    mov rbp, rsp
    and rsp, -16
    sub rsp, 20h
    call qword ptr [g_SetAirTauntATRisingSunCheckCall]
    mov rsp, rbp
    PopAllXmm
    cmp al, 1
    PopAllRegsNoFlags
    je ApplyATRisingSun
    jmp ReturnLabel

ApplyATRisingSun:
    mov word ptr [rcx+3fa4h],019h
    jmp CodeLabel

CodeLabel:
    call qword ptr g_SetAirTaunt_Call
ReturnLabel:
    jmp qword ptr [g_SetAirTaunt_ReturnAddr]

SetAirTauntDetour ENDP

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
    jmp CodeLabel

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
    jne ConditionalLabel2 ; Force taunt
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
