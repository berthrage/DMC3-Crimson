.DATA
extern g_CameraSensitivity_ReturnAddr1:QWORD
extern g_CameraSensitivity_ReturnAddr2:QWORD
extern g_CameraSensitivity_NewSensAddr:QWORD

.CODE
CameraSensitivityDetour1 PROC
    push r14
    mov r14, g_CameraSensitivity_NewSensAddr
    movaps xmm9, [r14]
    movups [rdi+1d4h], xmm9
    pop r14
    jmp qword ptr [g_CameraSensitivity_ReturnAddr1]

CameraSensitivityDetour1 ENDP

.CODE
CameraSensitivityDetour2 PROC
    push r14
    mov r14, g_CameraSensitivity_NewSensAddr
    movaps xmm9, [r14]
    movups [rdi+1d4h], xmm9
    pop r14
    jmp qword ptr [g_CameraSensitivity_ReturnAddr2]

CameraSensitivityDetour2 ENDP
END