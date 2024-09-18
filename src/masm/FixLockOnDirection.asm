.DATA
extern g_FixLockOnDirection_ReturnAddr:QWORD
extern g_FixLockOnDirectionCall:QWORD
extern g_FixLockOnDirection_GetRotationTowardsEnemyCall: QWORD
enemyCameraDirection dw 0

.CODE
FixLockOnDirectionDetour PROC
    ; player in rbx
    push rax
    push rbx
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11

    mov rcx, rbx
    call qword ptr [g_FixLockOnDirectionCall] ;
    mov word ptr [enemyCameraDirection], ax
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rbx
    pop rax
    cmp word ptr [rbx + 3E84h], 01 ; is player locking on?
    jmp SetEnemyCameraDirection
    mov word ptr [rbx + 750Ch],ax
    jmp qword ptr [g_FixLockOnDirection_ReturnAddr]

SetEnemyCameraDirection:
    push r9
    mov r9w, word ptr [enemyCameraDirection]
    mov word ptr [rbx + 750Ch],r9w
    pop r9
    jmp qword ptr [g_FixLockOnDirection_ReturnAddr]

FixLockOnDirectionDetour ENDP
END