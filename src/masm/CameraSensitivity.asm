.DATA
extern g_CameraSensitivity_ReturnAddr1:QWORD
extern g_CameraSensitivity_ReturnAddr2:QWORD
extern g_CameraSensitivity_NewSensAddr:QWORD

.CODE
CameraSensitivityDetour1 PROC
    push r14
    mov r14, g_CameraSensitivity_NewSensAddr

    sub     rsp, 20h             ; Allocate space for xmm1 (16 bytes), align stack
    movdqu  [rsp], xmm0          ; Save xmm1

    movups xmm0, [r14]
    movdqu  [rdi+1d4h], xmm0

    movdqu  xmm0, [rsp]          ; Restore xmm1
    add     rsp, 20h             ; Restore stack from xmm1 save

    pop r14
    jmp qword ptr [g_CameraSensitivity_ReturnAddr1]

CameraSensitivityDetour1 ENDP

.CODE
CameraSensitivityDetour2 PROC
    push r14
    mov r14, g_CameraSensitivity_NewSensAddr

    sub     rsp, 20h             ; Allocate space for xmm1 (16 bytes), align stack
    movdqu  [rsp], xmm0          ; Save xmm1

    movups xmm0, [r14]
    movdqu [rdi+1d4h], xmm0

    movdqu  xmm0, [rsp]          ; Restore xmm1
    add     rsp, 20h             ; Restore stack from xmm1 save

    pop r14
    jmp qword ptr [g_CameraSensitivity_ReturnAddr2]

CameraSensitivityDetour2 ENDP
END