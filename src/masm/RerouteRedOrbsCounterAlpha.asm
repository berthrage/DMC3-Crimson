.DATA
extern g_RerouteRedOrbsCounterAlpha_ReturnAddr1:QWORD
extern g_RerouteRedOrbsCounterAlpha_VariableAddr:QWORD

.CODE
RerouteRedOrbsCounterAlphaDetour1 PROC
    push r8
    mov r8, g_RerouteRedOrbsCounterAlpha_VariableAddr
    mov word ptr [r8], ax
    pop r8
    jmp qword ptr [g_RerouteRedOrbsCounterAlpha_ReturnAddr1]

RerouteRedOrbsCounterAlphaDetour1 ENDP

.DATA
extern g_RerouteRedOrbsCounterAlpha_ReturnAddr2:QWORD
extern g_RerouteRedOrbsCounterAlpha_VariableAddr:QWORD

.CODE
RerouteRedOrbsCounterAlphaDetour2 PROC
    push r8
    mov r8, g_RerouteRedOrbsCounterAlpha_VariableAddr
    mov word ptr [r8], bx
    pop r8
    jmp qword ptr [g_RerouteRedOrbsCounterAlpha_ReturnAddr2]

RerouteRedOrbsCounterAlphaDetour2 ENDP

.DATA
extern g_RerouteRedOrbsCounterAlpha_ReturnAddr3:QWORD
extern g_RerouteRedOrbsCounterAlpha_VariableAddr:QWORD

.CODE
RerouteRedOrbsCounterAlphaDetour3 PROC
    push r8
    mov r8, g_RerouteRedOrbsCounterAlpha_VariableAddr
    mov word ptr [r8], ax
    pop r8
    jmp qword ptr [g_RerouteRedOrbsCounterAlpha_ReturnAddr3]

RerouteRedOrbsCounterAlphaDetour3 ENDP

.DATA
extern g_RerouteRedOrbsCounterAlpha_ReturnAddr4:QWORD
extern g_RerouteRedOrbsCounterAlpha_VariableAddr:QWORD

.CODE
RerouteRedOrbsCounterAlphaDetour4 PROC
    push r8
    mov r8, g_RerouteRedOrbsCounterAlpha_VariableAddr
    movzx eax, word ptr [r8]
    pop r8
    jmp qword ptr [g_RerouteRedOrbsCounterAlpha_ReturnAddr4]

RerouteRedOrbsCounterAlphaDetour4 ENDP

END
