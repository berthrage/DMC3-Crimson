.DATA
extern g_ShootRemapDown_ReturnAddr:QWORD

.CODE
ShootRemapDownDetour PROC

    movzx eax,word ptr [rbp+rax*2+0Ah] 
    ; After division, the quotient is in EAX
    jmp OriginalCode

OriginalCode:
    movzx eax,word ptr [rbp+rax*2+0Ah]  
    jmp qword ptr [g_ShootRemapDown_ReturnAddr]


ShootRemapDownDetour ENDP
END