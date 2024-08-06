.DATA
extern g_ToggleTakeDamage_ReturnAddr:QWORD
extern g_ToggleTakeDamageCheckCall:QWORD

.CODE
ToggleTakeDamageDetour PROC
    mov r12, rax

    
    ; Load player addr + 0x4114 on rcx
    mov rcx, r12

    ; Preserve xmm0 
    movss xmm10, xmm0

    ; Call the C++ function
    call qword ptr [g_ToggleTakeDamageCheckCall]
   
    cmp al, 1
    jne TakeDamage
    jmp SkipDamage

TakeDamage:

    movss DWORD PTR [r12 + 08h], xmm10  
    jmp qword ptr [g_ToggleTakeDamage_ReturnAddr]

SkipDamage:
    jmp qword ptr [g_ToggleTakeDamage_ReturnAddr]

ToggleTakeDamageDetour ENDP
END
