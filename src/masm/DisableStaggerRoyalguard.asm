.DATA
extern g_DisableStagger_ReturnAddr:QWORD
extern g_DisableStagger_ConditionalAddr:QWORD
extern g_DisableStaggerCheckCall:QWORD

.CODE
DisableStaggerRoyalguardDetour PROC
    push rax
    push rcx

    ; Load player data into rcx 
    mov rcx, rsi

    ; Call the C++ function
    call qword ptr [g_DisableStaggerCheckCall]

    ; Restore registers
    pop rcx
    

    ; Check the result
    cmp al, 1
    pop rax
    jne ContinueOriginalCode

    ; Apply the cheat if the function returned true
     jmp ConditionalJump
    

ContinueOriginalCode:
    ; Restore the original code
    cmp eax, 01
    jng ConditionalJump
    jmp qword ptr [g_DisableStagger_ReturnAddr]

ConditionalJump:
    mov rax, qword ptr [g_DisableStagger_ConditionalAddr]
    jmp rax

DisableStaggerRoyalguardDetour ENDP
END
