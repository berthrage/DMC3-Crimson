.DATA
extern g_GuardBreakToNormalBlock_ReturnAddr:QWORD
extern g_GuardBreakToNormalBlockCheckCall:QWORD
minimumDTAmount dd 1500.0

.CODE
GuardBreakToNormalBlockDetour PROC
    ; player + 60 in rbx
    cmp eax, 06
    je CheckIfGuard
    jmp OriginalCode

CheckIfGuard:
    push r8
    mov r8b, byte ptr [rbx + 06340h - 60h] ; checking if in guard
    cmp r8b, 01
    pop r8
    je CheckIfNotStaggered
    jmp OriginalCode

CheckIfNotStaggered:
    push r8
    mov r8b, byte ptr [rbx + 03E00h - 60h] ; checking event
    cmp r8d, 44
    pop r8
    jne CheckIfEnoughDT
    jmp OriginalCode

CheckIfEnoughDT:
    sub rsp, 16                ; Allocate 16 bytes on the stack (128 bits) to preverse xmm0
    movups [rsp], xmm0  
    movss xmm0, dword ptr [rbx + 03EB8h - 60h]
    ucomiss xmm0, dword ptr [minimumDTAmount] ; comparing currentDT to minmumDTAmount
    jb OriginalCodeWRestoration
    jmp ModifyGuardBreak
    movups xmm0, [rsp]
    add rsp, 16
    
ModifyGuardBreak:
    movups xmm0, [rsp]
    add rsp, 16
    mov eax, 04
    jmp OriginalCode

OriginalCodeWRestoration:
    movups xmm0, [rsp] ; Restore xmm0
    add rsp, 16
    mov [rbx + 06308h], eax
    jmp qword ptr [g_GuardBreakToNormalBlock_ReturnAddr]
    
OriginalCode:
    mov [rbx + 06308h], eax
    jmp qword ptr [g_GuardBreakToNormalBlock_ReturnAddr]


GuardBreakToNormalBlockDetour ENDP
END
