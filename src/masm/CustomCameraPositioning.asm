.DATA
extern g_CustomCameraPos_ReturnAddr:QWORD
extern g_CustomCameraPos_NewPosAddr:QWORD

.CODE
CustomCameraPositioningDetour PROC
    push r9
    push rcx
    mov r9, g_CustomCameraPos_NewPosAddr
    movaps xmm0, [r9]
    pop rcx
    pop r9
    jmp qword ptr [g_CustomCameraPos_ReturnAddr]

CustomCameraPositioningDetour ENDP
END