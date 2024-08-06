.DATA
extern g_ToggleTakeDamage_ReturnAddr:QWORD
extern g_ToggleTakeDamageCheckCall:QWORD

.CODE
ToggleTakeDamageDetour PROC
    mov r12, rax
    push rcx
    
    ; Load player addr + 0x4114 on rcx
    mov rcx, r12

    ; Call the C++ function
    call qword ptr [g_ToggleTakeDamageCheckCall]
    pop rcx
    cmp al, 1
    jne TakeDamage
    jmp SkipDamage

TakeDamage:
    movss DWORD PTR [r12 + 08h], xmm0  
    jmp qword ptr [g_ToggleTakeDamage_ReturnAddr]

SkipDamage:
    jmp qword ptr [g_ToggleTakeDamage_ReturnAddr]

ToggleTakeDamageDetour ENDP
END
