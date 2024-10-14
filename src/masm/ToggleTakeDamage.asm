.DATA
extern g_ToggleTakeDamage_ReturnAddr:QWORD
extern g_ToggleTakeDamageCheckCall:QWORD
preservedFloat dd 0.0

.CODE
ToggleTakeDamageDetour PROC
    ; Preserve what's in rax (actor addr + 0x4114) on r12
    push r12
    push rcx
    mov r12, rax

    ; Load player addr + 0x4114 on rcx
    mov rcx, r12

    ; Preserve xmm0 since the CheckCall will use it for player's DT comparison
    movups [preservedFloat], xmm0

    ; Call the C++ function
    call qword ptr [g_ToggleTakeDamageCheckCall]
   
    cmp al, 1
    jne TakeDamage
    jmp SkipDamage

TakeDamage:
    movups xmm0, [preservedFloat]
    movss DWORD PTR [r12 + 08h], xmm0  
    pop rcx
    pop r12
    jmp qword ptr [g_ToggleTakeDamage_ReturnAddr]

SkipDamage:
    movups xmm0, [preservedFloat]
    pop rcx
    pop r12
    jmp qword ptr [g_ToggleTakeDamage_ReturnAddr]

ToggleTakeDamageDetour ENDP
END
